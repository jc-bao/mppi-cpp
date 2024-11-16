#include <cstdio>
#include <cstring>
#include <chrono>
#include <thread>
#include <vector>
#include <random>
#include <iostream>
#include <GLFW/glfw3.h>
#include <mujoco/mujoco.h>

#define REF_FREQ 2.0
#define REF_SCALE 0.6
#define PI 3.14159265358979323846

// maximum number of threads
const int maxthread = 512;
const int maxstep = 1000;

// Helper function to clip value between min and max
double clip(double value, double min_val, double max_val)
{
    if (value < min_val)
        return min_val;
    if (value > max_val)
        return max_val;
    return value;
}

// Get reference joint angles at time t
// ref_joint: output array to store the 8 joint angles
// t: current time
void get_ref_joint(double *ref_joint, double t)
{
    // Calculate base sine wave
    double base_sine = sin(2.0 * PI * t * REF_FREQ) * REF_SCALE;

    // Calculate clipped delta angles for left and right
    double delta_angle_left = clip(base_sine, -1.0, 1.0);
    double delta_angle_right = clip(-base_sine, -1.0, 1.0);

    // Fill the reference joint array
    // [left_hip_pitch, left_hip_roll, left_knee, left_ankle_pitch,
    //  right_hip_pitch, right_hip_roll, right_knee, right_ankle_pitch]
    ref_joint[0] = -delta_angle_left;  // left hip pitch
    ref_joint[1] = 0.0;                      // left hip roll
    ref_joint[2] = delta_angle_left;         // left knee
    ref_joint[3] = 0.0;                      // left ankle pitch
    ref_joint[4] = -delta_angle_right; // right hip pitch
    ref_joint[5] = 0.0;                      // right hip roll
    ref_joint[6] = delta_angle_right;        // right knee
    ref_joint[7] = 0.0;                      // right ankle pitch
}

// model and per-thread data
mjModel *m = NULL;
mjData *d_main = NULL; // Main simulation environment
mjData *d[maxthread];

// per-thread statistics
mjtNum simtime[maxthread];

// per-thread cumulative reward and base position (along the trajectory)
double cumulative_reward[maxthread];
double base_position[maxthread][maxstep][3];

// Compute softmax probabilities with temperature
std::vector<double> compute_softmax_probabilities(const double *rewards, int n, double temperature)
{
    std::vector<double> probabilities(n);

    // normalize rewards with mean and std
    double mean = 0.0;
    double std = 0.0;
    std::vector<double> normalized_rewards(n);
    for (int i = 0; i < n; i++)
    {
        mean += rewards[i];
    }
    mean /= n;
    for (int i = 0; i < n; i++)
    {
        std += (rewards[i] - mean) * (rewards[i] - mean);
    }
    std = std::sqrt(std / n);
    for (int i = 0; i < n; i++)
    {
        normalized_rewards[i] = (rewards[i] - mean) / std;
    }

    // Compute exp(x/T) for each reward
    double sum = 0.0;
    for (int i = 0; i < n; i++)
    {
        probabilities[i] = std::exp(normalized_rewards[i] / temperature);
        sum += probabilities[i];
    }

    // Normalize to get probabilities
    for (int i = 0; i < n; i++)
    {
        probabilities[i] /= sum;
    }

    return probabilities;
}

// Weighted average of actions based on softmax probabilities
void compute_weighted_action(const std::vector<std::vector<mjtNum>> &actions,
                             const std::vector<double> &probabilities,
                             std::vector<mjtNum> &result)
{
    int action_dim = actions[0].size();
    result.resize(action_dim, 0.0);

    // Compute weighted sum of actions
    for (size_t i = 0; i < actions.size(); i++)
    {
        for (int j = 0; j < action_dim; j++)
        {
            result[j] += actions[i][j] * probabilities[i];
        }
    }
}

// Define the joint ranges as constants
const double JOINT_RANGES[8][2] = {
    {-1.0, 0.2}, // left hip pitch
    {-0.1, 0.1}, // left hip roll
    {0.7, 1.7},  // left knee
    {-0.8, 0.6}, // left ankle pitch
    {-1.0, 0.2}, // right hip pitch
    {-0.1, 0.1}, // right hip roll
    {0.7, 1.7},  // right knee
    {-0.8, 0.6}  // right ankle pitch
};

// Convert a single value from [-1, 1] to target range [min, max]
double convert_to_range(double value, double min_val, double max_val)
{
    // Ensure input is clamped to [-1, 1]
    value = mjMAX(-1.0, mjMIN(1.0, value));

    // Convert from [-1, 1] to [0, 1]
    double normalized = (value + 1.0) / 2.0;

    // Convert to target range
    return min_val + normalized * (max_val - min_val);
}

// Convert entire action array from [-1, 1] to joint ranges
void convert_action_to_joint_ranges(double t, const double *raw_action, double *converted_action, int num_joints = 8)
{
    for (int i = 0; i < num_joints; i++)
    {
        converted_action[i] = convert_to_range(
            raw_action[i],
            JOINT_RANGES[i][0],
            JOINT_RANGES[i][1]);
    }

    // add reference joint angles to the action
    double ref_joint[8];
    get_ref_joint(ref_joint, t);
    for (int i = 0; i < num_joints; i++)
    {
        converted_action[i] += ref_joint[i];
    }

    // clip action to joint ranges
    for (int i = 0; i < num_joints; i++)
    {
        converted_action[i] = mjMIN(mjMAX(converted_action[i], JOINT_RANGES[i][0]), JOINT_RANGES[i][1]);
    }
}

// Optional: Convert back from joint ranges to [-1, 1] range
double convert_from_range(double value, double min_val, double max_val)
{
    // Ensure input is clamped to [min_val, max_val]
    value = mjMAX(min_val, mjMIN(max_val, value));

    // Convert to [0, 1]
    double normalized = (value - min_val) / (max_val - min_val);

    // Convert to [-1, 1]
    return normalized * 2.0 - 1.0;
}

// Helper function to get color based on reward value
void get_reward_color(double reward, double min_reward, double max_reward, float color[4])
{
    // Normalize reward to [0,1]
    double normalized = (reward - min_reward) / (max_reward - min_reward);

    // Create a color gradient from blue (cold) to red (hot)
    color[0] = normalized;       // Red
    color[1] = 0.0;              // Green
    color[2] = 1.0 - normalized; // Blue
    color[3] = 1.0;              // Alpha
}

// Function to visualize trajectories
void visualize_trajectories(mjvScene *scn, const double base_position[maxthread][maxstep][3],
                            const double cumulative_reward[maxthread], int nthread, int nstep)
{
    // Find min and max rewards for color scaling
    double min_reward = cumulative_reward[0];
    double max_reward = cumulative_reward[0];
    for (int id = 0; id < nthread; id++)
    {
        min_reward = mjMIN(min_reward, cumulative_reward[id]);
        max_reward = mjMAX(max_reward, cumulative_reward[id]);
    }

    // Width of the trajectory line
    double width = 0.005;

    // Scratch arrays for initialization
    double zero3[3] = {0};
    double zero9[9] = {0};

    // Draw trajectories for each thread
    for (int id = 0; id < nthread; id++)
    {
        // Get color based on cumulative reward
        float color[4];
        get_reward_color(cumulative_reward[id], min_reward, max_reward, color);

        // Draw line segments connecting positions
        for (int step = 0; step < nstep - 1; step++)
        {
            // Check if we've reached maximum geometries
            if (scn->ngeom >= scn->maxgeom)
            {
                printf("Warning: Reached maximum geometry count\n");
                return;
            }

            // Initialize geometry
            mjv_initGeom(&scn->geoms[scn->ngeom], mjGEOM_CAPSULE, zero3, zero3, zero9, color);

            // Create connector between consecutive points
            mjv_connector(&scn->geoms[scn->ngeom], mjGEOM_CAPSULE, width,
                          base_position[id][step],
                          base_position[id][step + 1]);

            // Increment geometry count
            scn->ngeom++;
        }
    }
}

// timer
mjtNum gettm(void)
{
    using std::chrono::steady_clock;
    using Microseconds = std::chrono::duration<double, std::micro>;
    static steady_clock::time_point tm_start = steady_clock::now();
    auto elapsed = Microseconds(steady_clock::now() - tm_start);
    return elapsed.count();
}

// GLFW window and rendering data
GLFWwindow *window = NULL;
mjvCamera cam;  // abstract camera
mjvOption opt;  // visualization options
mjvScene scn;   // abstract scene
mjrContext con; // custom GPU context

// mouse interaction
bool button_left = false;
bool button_middle = false;
bool button_right = false;
double lastx = 0;
double lasty = 0;

// keyboard callback
void keyboard(GLFWwindow *window, int key, int scancode, int act, int mods)
{
    // backspace: reset simulation
    if (act == GLFW_PRESS && key == GLFW_KEY_BACKSPACE)
    {
        mj_resetData(m, d_main);
        mj_forward(m, d_main);
    }
}

// mouse button callback
void mouse_button(GLFWwindow *window, int button, int act, int mods)
{
    // update button state
    button_left = (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS);
    button_middle = (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_MIDDLE) == GLFW_PRESS);
    button_right = (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_RIGHT) == GLFW_PRESS);

    // update mouse position
    glfwGetCursorPos(window, &lastx, &lasty);
}

// mouse move callback
void mouse_move(GLFWwindow *window, double xpos, double ypos)
{
    // no buttons down: nothing to do
    if (!button_left && !button_middle && !button_right)
    {
        return;
    }

    // compute mouse displacement, save
    double dx = xpos - lastx;
    double dy = ypos - lasty;
    lastx = xpos;
    lasty = ypos;

    // get current window size
    int width, height;
    glfwGetWindowSize(window, &width, &height);

    // get shift key state
    bool mod_shift = (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS ||
                      glfwGetKey(window, GLFW_KEY_RIGHT_SHIFT) == GLFW_PRESS);

    // determine action based on mouse button
    mjtMouse action;
    if (button_right)
    {
        action = mod_shift ? mjMOUSE_MOVE_H : mjMOUSE_MOVE_V;
    }
    else if (button_left)
    {
        action = mod_shift ? mjMOUSE_ROTATE_H : mjMOUSE_ROTATE_V;
    }
    else
    {
        action = mjMOUSE_ZOOM;
    }

    // move camera
    mjv_moveCamera(m, action, dx / height, dy / height, &scn, &cam);
}

// scroll callback
void scroll(GLFWwindow *window, double xoffset, double yoffset)
{
    // emulate vertical mouse motion = 5% of window height
    mjv_moveCamera(m, mjMOUSE_ZOOM, 0, -0.05 * yoffset, &scn, &cam);
}

// deallocate and print message
int finish(const char *msg = NULL, mjModel *m = NULL)
{
    // deallocate model
    if (m)
    {
        mj_deleteModel(m);
    }

    // terminate GLFW
    if (window)
    {
        glfwDestroyWindow(window);
        glfwTerminate();
    }

    // print message
    if (msg)
    {
        std::printf("%s\n", msg);
    }

    return 0;
}

// thread function
void simulate(int id, int nstep, mjtNum *action)
{
    // initialize cumulative reward
    cumulative_reward[id] = 0.0;
    double *converted_action = new double[m->nu];

    // run and time
    for (int i = 0; i < nstep; i++)
    {
        // convert action to control
        convert_action_to_joint_ranges(d[id]->time, action, converted_action);

        // set control to action
        mju_copy(d[id]->ctrl, converted_action, m->nu);

        // advance simulation
        mj_step(m, d[id]);

        // get id of "torso_link"
        int torso_id = mj_name2id(m, mjOBJ_BODY, "torso_link");
        mjtNum *xpos = d[id]->xpos + 3 * torso_id;
        mjtNum *xquat = d[id]->xquat + 4 * torso_id;
        mjtNum *cvel = d[id]->cvel + 3 * torso_id;

        // get id of left_ankle_roll_link and right_ankle_roll_link
        int left_ankle_roll_id = mj_name2id(m, mjOBJ_BODY, "left_ankle_roll_link");
        int right_ankle_roll_id = mj_name2id(m, mjOBJ_BODY, "right_ankle_roll_link");
        mjtNum *left_ankle_roll_pos = d[id]->xpos + 3 * left_ankle_roll_id;
        mjtNum *right_ankle_roll_pos = d[id]->xpos + 3 * right_ankle_roll_id;

        // compute reward (example: negative squared distance from origin)
        // height reward, target is 0.78, use squared distance
        // set target position to average of left and right ankle roll position in x-y plane
        mjtNum pos_tar[3] = {(left_ankle_roll_pos[0] + right_ankle_roll_pos[0]) / 2.0,
                             (left_ankle_roll_pos[1] + right_ankle_roll_pos[1]) / 2.0, 0.78};
        mjtNum reward_pos = -mju_dist3(xpos, pos_tar);
        // height reward, target is 0.78, use squared distance
        mjtNum reward_height = -mju_abs(xpos[2] - 0.78);
        // appy rotation matrix to [0,0,1] to get new vector, then use squre distance
        mjtNum vec[3] = {0, 0, 1};
        mjtNum res[3];
        mju_rotVecQuat(res, vec, xquat);
        mjtNum reward_upright = -mju_dist3(res, vec);
        // velocity reward, target is [1.0, 0.0, 0.0], use squared distance
        mjtNum vec_vel[3] = {2.0, 0.0, 0.0};
        mjtNum reward_vel = -mju_dist3(cvel, vec_vel);

        // accumulate reward
        cumulative_reward[id] += (1.0 * reward_pos + 1.0 * reward_height + 0.3 * reward_upright + 0.0 * reward_vel);

        // record position
        base_position[id][i][0] = xpos[0];
        base_position[id][i][1] = xpos[1];
        base_position[id][i][2] = xpos[2];
    }
}

// main function
int main(int argc, char **argv)
{
    // print help if arguments are missing
    if (argc < 2 || argc > 5)
    {
        return finish(
            "\n"
            "Usage:  testspeed modelfile [nstep nthread]\n"
            "\n"
            "  argument      default     semantic\n"
            "  --------      -------     --------\n"
            "  modelfile                 path to model (required)\n"
            "  nstep         100         number of steps per rollout\n"
            "  nthread       4           number of threads running parallel rollouts\n"
            "  nrepeat       10          number of times to repeat the rollout\n");
    }

    // read arguments
    int nstep = 100, nthread = 4, nrepeat = 10;
    if (argc > 2 && (std::sscanf(argv[2], "%d", &nstep) != 1 || nstep <= 0))
    {
        return finish("Invalid nstep argument");
    }
    if (argc > 3 && std::sscanf(argv[3], "%d", &nthread) != 1)
    {
        return finish("Invalid nthread argument");
    }
    if (argc > 4 && std::sscanf(argv[4], "%d", &nrepeat) != 1)
    {
        return finish("Invalid nrepeat argument");
    }
    // clamp nstep to [1, maxstep]
    if (nstep > maxstep)
    {
        std::printf("Clamping nstep to %d\n", maxstep);
        nstep = maxstep;
    }

    // clamp nthread to [1, maxthread]
    nthread = mjMAX(1, mjMIN(maxthread, nthread));

    // get filename, determine file type
    std::string filename(argv[1]);
    bool binary = (filename.find(".mjb") != std::string::npos);

    // load model
    char error[1000] = "Could not load binary model";
    if (binary)
    {
        m = mj_loadModel(argv[1], 0);
    }
    else
    {
        m = mj_loadXML(argv[1], 0, error, 1000);
    }
    if (!m)
    {
        return finish(error);
    }

    // make main data
    d_main = mj_makeData(m);
    if (!d_main)
    {
        return finish("Could not allocate mjData for main environment", m);
    }

    // make per-thread data
    for (int id = 0; id < nthread * nrepeat; id++)
    {
        // make mjData(s)
        d[id] = mj_makeData(m);
        if (!d[id])
        {
            return finish("Could not allocate mjData", m);
        }
    }

    // install timer callback for profiling
    mjcb_time = gettm;

    // Initialize GLFW
    if (!glfwInit())
    {
        return finish("Could not initialize GLFW", m);
    }

    // create window, make OpenGL context current, request v-sync
    window = glfwCreateWindow(1200, 900, "Simulation", NULL, NULL);
    if (!window)
    {
        return finish("Could not create GLFW window", m);
    }
    glfwMakeContextCurrent(window);
    glfwSwapInterval(1);

    // initialize visualization data structures
    mjv_defaultCamera(&cam);
    mjv_defaultOption(&opt);
    mjv_defaultScene(&scn);
    mjr_defaultContext(&con);

    // create scene and context
    mjv_makeScene(m, &scn, 2000);
    mjr_makeContext(m, &con, mjFONTSCALE_150);

    // install GLFW mouse and keyboard callbacks
    glfwSetKeyCallback(window, keyboard);
    glfwSetCursorPosCallback(window, mouse_move);
    glfwSetMouseButtonCallback(window, mouse_button);
    glfwSetScrollCallback(window, scroll);

    // Use a random number generator
    std::random_device rd;
    std::mt19937 gen(rd());
    // sample from Gaussian distribution with mean 0 and standard deviation 0.5
    std::normal_distribution<mjtNum> dis(0.0, 1.0);

    // generate actions for each thread
    std::vector<std::vector<mjtNum>> actions(nthread * nrepeat, std::vector<mjtNum>(m->nu));

    // generate actions for main thread
    std::vector<mjtNum> main_action(m->nu);
    std::vector<mjtNum> converted_main_action(m->nu);

    // run simulation loop
    std::vector<std::thread> th(nthread * nrepeat);

    while (!glfwWindowShouldClose(window))
    {
        // Handle GLFW events
        glfwPollEvents();

        // Copy d_main into each d[id]
        for (int id = 0; id < nthread * nrepeat; id++)
        {
            mj_copyData(d[id], m, d_main);
        }

        // Generate random actions for each thread
        for (int id = 0; id < nthread * nrepeat; id++)
        {
            // sample action from random noise
            for (int i = 0; i < m->nu; i++)
            {
                actions[id][i] = dis(gen) + main_action[i];
                // smooth action
                actions[id][i] = 0.5 * actions[id][i] + 0.5 * main_action[i];
                // clamp action to [-1, 1]
                actions[id][i] = mjMIN(mjMAX(actions[id][i], -1.0), 1.0);
            }
            cumulative_reward[id] = 0.0; // Reset cumulative reward
        }

        // Run simulation threads
        // time the simulation
        auto start_time = std::chrono::high_resolution_clock::now();
        for (int repeat = 0; repeat < nrepeat; repeat++)
        {
            for (int id = repeat * nthread; id < (repeat + 1) * nthread; id++)
            {
                th[id] = std::thread(simulate, id, nstep, actions[id].data());
            }

            for (int id = repeat * nthread; id < (repeat + 1) * nthread; id++)
            {
                th[id].join();
            }
        }
        auto end_time = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end_time - start_time);
        std::cout << "planning frequency: " << 1000.0 / duration.count() << " Hz" << std::endl;

        // update main_action with softmax probabilities
        std::vector<double> probabilities = compute_softmax_probabilities(cumulative_reward, nthread * nrepeat, 0.1);
        compute_weighted_action(actions, probabilities, main_action);

        // convert action to control
        // std::fill(main_action.begin(), main_action.end(), 0.0); // DEBUG
        convert_action_to_joint_ranges(d_main->time, main_action.data(), converted_main_action.data(), m->nu);

        // Apply the best action to d_main
        mju_copy(d_main->ctrl, converted_main_action.data(), m->nu);

        // Simulate d_main forward for one step
        mj_step(m, d_main);

        // check if the robot is falling
        int torso_id = mj_name2id(m, mjOBJ_BODY, "torso_link");
        if (d_main->xpos[3 * torso_id + 2] < 0.6)
        {
            std::cout << "Robot is falling" << std::endl;
            // reset the robot to the initial position
            mj_resetData(m, d_main);
            mj_forward(m, d_main);
        }

        // shift forward main_action by appending zero to the end and remove the first element
        main_action.insert(main_action.end(), 0.0);
        main_action.erase(main_action.begin());

        // get framebuffer viewport
        mjrRect viewport = {0, 0, 0, 0};
        glfwGetFramebufferSize(window, &viewport.width, &viewport.height);

        // update scene and render
        mjv_updateScene(m, d_main, &opt, NULL, &cam, mjCAT_ALL, &scn);
        // Visualize trajectories
        visualize_trajectories(&scn, base_position, cumulative_reward, nthread, nstep);
        // render
        mjr_render(viewport, &scn, &con);

        // swap OpenGL buffers (blocking call due to v-sync)
        glfwSwapBuffers(window);

        // process pending GUI events, call GLFW callbacks
        glfwPollEvents();
    }

    // free per-thread data
    for (int id = 0; id < nthread; id++)
    {
        mj_deleteData(d[id]);
    }

    // free main data
    mj_deleteData(d_main);

    // free visualization storage
    mjv_freeScene(&scn);
    mjr_freeContext(&con);

    // finalize
    return finish();
}
