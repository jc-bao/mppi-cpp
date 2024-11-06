#include <GLFW/glfw3.h>
#include <absl/base/attributes.h>
#include <absl/random/random.h>
#include <mujoco/mujoco.h>

#include <chrono>
#include <condition_variable>
#include <cstdint>
#include <cstdio>
#include <cstring>
#include <filesystem>
#include <fstream>
#include <functional>
#include <iostream>
#include <memory>
#include <mutex>
#include <queue>
#include <string>
#include <thread>
#include <unordered_map>
#include <vector>

// -- visualization global -- //
mjvCamera cam;   // abstract camera
mjvOption opt;   // visualization options
mjvScene scn;    // abstract scene
mjrContext con;  // custom GPU context

// -- utility methods from github.com/google-deepmind/mujoco_mpc -- //

// check mjData for warnings, return true if any warnings
bool CheckWarnings(mjData* data) {
  bool warnings_found = false;
  for (int i = 0; i < mjNWARNING; i++) {
    if (data->warning[i].number > 0) {
      // reset
      data->warning[i].number = 0;

      // return failure
      warnings_found = true;
    }
  }
  return warnings_found;
}

// ThreadPool class
class ThreadPool {
 public:
  // constructor
  explicit ThreadPool(int num_threads) : ctr_(0) {
    for (int i = 0; i < num_threads; i++) {
      threads_.push_back(std::thread(&ThreadPool::WorkerThread, this, i));
    }
  }

  // destructor
  ~ThreadPool() {
    {
      std::unique_lock<std::mutex> lock(m_);
      for (int i = 0; i < threads_.size(); i++) {
        queue_.push(nullptr);
      }
      cv_in_.notify_all();
    }
    for (auto& thread : threads_) {
      thread.join();
    }
  }

  int NumThreads() const { return threads_.size(); }

  // ----- methods ----- //
  // set task for threadpool
  void Schedule(std::function<void()> task) {
    std::unique_lock<std::mutex> lock(m_);
    queue_.push(std::move(task));
    cv_in_.notify_one();
  }

  // return number of tasks completed
  std::uint64_t GetCount() { return ctr_; }

  // reset count to zero
  void ResetCount() { ctr_ = 0; }

  // wait for count, then return
  void WaitCount(int value) {
    std::unique_lock<std::mutex> lock(m_);
    cv_ext_.wait(lock, [&]() { return this->GetCount() >= value; });
  }

 private:
  // ----- methods ----- //

  // execute task with available thread
  // ThreadPool worker
  void WorkerThread(int i) {
    // worker_id_ = i;
    while (true) {
      auto task = [&]() {
        std::unique_lock<std::mutex> lock(m_);
        cv_in_.wait(lock, [&]() { return !queue_.empty(); });
        std::function<void()> task = std::move(queue_.front());
        queue_.pop();
        cv_in_.notify_one();
        return task;
      }();
      if (task == nullptr) {
        {
          std::unique_lock<std::mutex> lock(m_);
          ++ctr_;
          cv_ext_.notify_one();
        }
        break;
      }
      task();

      {
        std::unique_lock<std::mutex> lock(m_);
        ++ctr_;
        cv_ext_.notify_one();
      }
    }
  }

  // ----- members ----- //
  std::vector<std::thread> threads_;
  std::mutex m_;
  std::condition_variable cv_in_;
  std::condition_variable cv_ext_;
  std::queue<std::function<void()>> queue_;
  std::uint64_t ctr_;
};

// get duration in seconds since time point
template <typename T>
T GetDuration(std::chrono::steady_clock::time_point time) {
  return 1.0e-3 * std::chrono::duration_cast<std::chrono::milliseconds>(
                      std::chrono::steady_clock::now() - time)
                      .count();
}

// load model from path
mjModel* LoadTestModel(std::string path) {
  // load model
  char loadError[1024] = "";
  mjModel* model = mj_loadXML(path.c_str(), nullptr, loadError, 1000);
  if (loadError[0]) std::cerr << "load error: " << loadError << '\n';

  return model;
}

// -- running statistics -- //

// https://www.johndcook.com/blog/standard_deviation/
template <typename T>
class RunningStatistics {
 public:
  RunningStatistics(int dim) {
    // initialize
    dim_ = dim;
    count_ = 0;
    m_old_.resize(dim_);
    m_new_.resize(dim_);
    s_old_.resize(dim_);
    s_new_.resize(dim_);
    var_.resize(dim_);
    std_.resize(dim_);
  }

  // reset count
  void Reset() { count_ = 0; }

  // update
  void Update(const std::vector<T>& x) {
    // increment count
    unsigned long int old_count = count_;
    count_++;

    // initialize
    if (count_ == 1) {
      m_old_ = x;
      m_new_ = x;
      std::fill(s_old_.begin(), s_old_.end(), 0.0);
      std::fill(s_new_.begin(), s_new_.end(), 0.0);
    } else {
      for (int i = 0; i < dim_; i++) {
        T delta = x[i] - m_old_[i];
        m_new_[i] = m_old_[i] + delta / count_;
        s_new_[i] = s_old_[i] + delta * delta * old_count / count_;
      }
      m_old_ = m_new_;
      s_old_ = s_new_;
    }
  }

  // update
  void Update(T x) {
    std::vector<T> vec;
    vec.push_back(x);
    Update(vec);
  }

  // identity initialization
  void InitializeIdentity() {
    Reset();
    count_ = 1;

    std::fill(m_old_.begin(), m_old_.end(), 0.0);
    std::fill(m_new_.begin(), m_new_.end(), 0.0);

    std::fill(s_old_.begin(), s_old_.end(), 1.0);
    std::fill(s_new_.begin(), s_new_.end(), 1.0);
  }

  // number of data points
  int Count() { return count_; }

  // mean
  const std::vector<T>& Mean() { return m_new_; }

  // variance
  const std::vector<T>& Variance() {
    for (int i = 0; i < dim_; i++) {
      var_[i] = count_ > 1 ? s_new_[i] / (count_ - 1) : 1.0;
    }
    return var_;
  }

  // standard deviation
  const std::vector<T>& StandardDeviation() {
    // compute variance
    Variance();

    // compute standard deviation
    for (int i = 0; i < dim_; i++) {
      std_[i] = (var_[i] > 1.0e-8) ? std::sqrt(var_[i]) : 1.0;
    }
    return std_;
  }

  // merge running statistics
  void Merge(const RunningStatistics& other) {
    unsigned long int count = this->count_ + other.count_;

    for (int i = 0; i < this->dim_; i++) {
      T delta = this->m_new_[i] - other.m_new_[i];
      T delta2 = delta * delta;

      T mean =
          (this->count_ * this->m_new_[i] + other.count_ * other.m_new_[i]) /
          count;
      T var = this->s_new_[i] + other.s_new_[i] +
              delta2 * this->count_ * other.count_ / count;

      this->m_old_[i] = mean;
      this->m_new_[i] = mean;
      this->s_old_[i] = var;
      this->s_new_[i] = var;
    }
    this->count_ = count;
  }

 private:
  int dim_;
  unsigned long int count_;
  std::vector<T> m_old_;
  std::vector<T> m_new_;
  std::vector<T> s_old_;
  std::vector<T> s_new_;
  std::vector<T> var_;
  std::vector<T> std_;
};

// -- policy -- //
// linear feedback policy
template <typename T>
class Policy {
 public:
  // default constructor
  Policy(int naction, int nobservation, const std::vector<T>& lower,
         const std::vector<T>& upper)
      : lower_(lower), upper_(upper) {
    // initialize
    naction_ = naction;
    nobservation_ = nobservation;

    // create
    weight_.resize(naction * nobservation);
    shift_.resize(nobservation);
    scale_.resize(nobservation);
    normalized_observation_.resize(nobservation);

    // initialize
    std::fill(weight_.begin(), weight_.end(), 0.0);
    std::fill(shift_.begin(), shift_.end(), 0.0);
    std::fill(scale_.begin(), scale_.end(), 1.0);
  }

  // evaluate policy
  void Evaluate(std::vector<T>& action,
                const std::vector<T>& observation) const {
    // normalize input
    for (int i = 0; i < nobservation_; i++) {
      normalized_observation_[i] =
          (std::abs(scale_[i]) < 1.0e-7)
              ? 0.0
              : (observation[i] - shift_[i]) / (scale_[i] + 1.0e-5);
    }

    // feedback matrix
    mju_mulMatVec(action.data(), weight_.data(), normalized_observation_.data(),
                  naction_, nobservation_);

    // clamp action
    for (int i = 0; i < naction_; i++) {
      action[i] = mju_clip(action[i], lower_[i], upper_[i]);
    }
  }

  // update policy
  void Update(const std::vector<T>& weight, const std::vector<T>& shift,
              const std::vector<T>& scale) {
    weight_ = weight;
    shift_ = shift;
    scale_ = scale;
  }

  // perturb weights
  void PerturbWeights(const std::vector<T>& perturb, T scale) {
    // sample individual weights
    for (int i = 0; i < naction_ * nobservation_; i++) {
      weight_[i] += scale * perturb[i];
    }
  }

  // save
  // chatgpt code
  void Save(const std::string& filename) const {
    std::ofstream ofs(filename, std::ios::binary);
    if (!ofs) {
      std::cerr << "Error opening file for writing: " << filename << std::endl;
      return;
    }

    // weight
    size_t weight_size = weight_.size();
    ofs.write(reinterpret_cast<const char*>(&weight_size), sizeof(weight_size));
    ofs.write(reinterpret_cast<const char*>(weight_.data()),
              weight_size * sizeof(T));

    // shift
    size_t shift_size = shift_.size();
    ofs.write(reinterpret_cast<const char*>(&shift_size), sizeof(shift_size));
    ofs.write(reinterpret_cast<const char*>(shift_.data()),
              shift_size * sizeof(T));

    // scale
    size_t scale_size = scale_.size();
    ofs.write(reinterpret_cast<const char*>(&scale_size), sizeof(scale_size));
    ofs.write(reinterpret_cast<const char*>(scale_.data()),
              scale_size * sizeof(T));

    ofs.close();
  }

  // load
  // chatgpt code
  void Load(const std::string& filename) {
    std::ifstream ifs(filename, std::ios::binary);
    if (!ifs) {
      std::cerr << "Error opening file for reading: " << filename << std::endl;
      return;
    }

    // weight
    size_t weight_size;
    ifs.read(reinterpret_cast<char*>(&weight_size), sizeof(weight_size));

    weight_.resize(weight_size);
    ifs.read(reinterpret_cast<char*>(weight_.data()), weight_size * sizeof(T));

    // shift
    size_t shift_size;
    ifs.read(reinterpret_cast<char*>(&shift_size), sizeof(shift_size));

    shift_.resize(shift_size);
    ifs.read(reinterpret_cast<char*>(shift_.data()), shift_size * sizeof(T));

    // scale
    size_t scale_size;
    ifs.read(reinterpret_cast<char*>(&scale_size), sizeof(scale_size));

    scale_.resize(scale_size);
    ifs.read(reinterpret_cast<char*>(scale_.data()), scale_size * sizeof(T));

    ifs.close();
  }

  // return data
  const std::vector<T>& Weight() { return weight_; }
  const std::vector<T>& Shift() { return shift_; }
  const std::vector<T>& Scale() { return scale_; }

  // sizes
  int NumAction() { return naction_; }
  int NumObservation() { return nobservation_; }
  int NumWeight() { return naction_ * nobservation_; }

 private:
  int naction_;                 // number of actions
  int nobservation_;            // number of observations
  const std::vector<T> lower_;  // action lower limits
  const std::vector<T> upper_;  // action upper limits
  std::vector<T> weight_;       // weights for feedback matrix
  std::vector<T> shift_;        // input shift
  std::vector<T> scale_;        // input scaling
  mutable std::vector<T> normalized_observation_;  // normalized observation
};

// -- environments -- //

// environment
template <typename T>
class Environment {
 public:
  // default constructor
  Environment(int naction, int nobservation)
      : naction_(naction), nobservation_(nobservation) {}

  // step environment
  virtual void Step(T* observation, T* reward, int* done, const T* action) = 0;

  // reset environment
  virtual void Reset(T* observation) = 0;

  // clone
  virtual std::unique_ptr<Environment<T>> Clone() const = 0;

  // visualize
  virtual void Visualize(const Policy<T>& p, int steps){};

  // environment dimensions
  int NumAction() const { return naction_; }
  int NumObservation() const { return nobservation_; }

 protected:
  int naction_;
  int nobservation_;
};

// MuJoCo environment
template <typename T>
class MuJoCoEnvironment : public Environment<T> {
 public:
  // default constructor
  explicit MuJoCoEnvironment(mjModel* model)
      : Environment<T>(model->nu, model->nq + model->nv),
        model_(mj_copyModel(nullptr, model)),
        data_(mj_makeData(model)),
        ndecimation_(1) {}

  // copy constructor
  MuJoCoEnvironment(const MuJoCoEnvironment<T>& env)
      : Environment<T>(env.NumAction(), env.NumObservation()),
        model_(mj_copyModel(nullptr, env.model_)),
        data_(mj_makeData(env.model_)),
        ndecimation_(env.ndecimation_) {}

  // default destructor
  ~MuJoCoEnvironment() {
    if (data_) {
      mj_deleteData(data_);
    }
    if (model_) {
      mj_deleteModel(model_);
    }
  };

  // visualize policy
  // https://github.com/google-deepmind/mujoco/blob/main/doc/programming/visualization.rst#visualization
  void Visualize(const Policy<T>& p, int steps) override {
    // MuJoCo data structures
    mjModel* m = Model();
    mjData* d = Data();
    std::vector<T> action(this->NumAction());  // actions from policy
    std::vector<T> observation(
        this->NumObservation());  // observation from environment
    std::vector<T> reward(1);
    std::vector<int> done(1);

    // init GLFW, create window, make OpenGL context current, request v-sync
    if (!glfwInit()) {
      mju_error("Could not initialize GLFW");
    }

    GLFWwindow* window =
        glfwCreateWindow(2400, 1800, "random search", NULL, NULL);
    glfwMakeContextCurrent(window);
    glfwSwapInterval(1);

    // initialize visualization data structures
    mjv_defaultCamera(&cam);
    mjv_defaultOption(&opt);
    mjr_defaultContext(&con);

    // create scene and context
    mjv_makeScene(m, &scn, 1000);
    mjr_makeContext(m, &con, mjFONTSCALE_100);

    // ... install GLFW keyboard and mouse callbacks

    // environment reset
    this->Reset(observation.data());

    // simulate physics
    auto render_start = std::chrono::steady_clock::now();
    T physics_start = d->time;

    // run main loop, target real-time simulation and 60 fps rendering
    T duration = m->opt.timestep * this->NumDecimation() * steps;
    while (!glfwWindowShouldClose(window) && d->time < duration) {
      // simulate physics
      auto start = std::chrono::steady_clock::now();

      // action from policy
      p.Evaluate(action, observation);

      // step environment
      this->Step(observation.data(), reward.data(), done.data(), action.data());

      // track
      if (GetDuration<double>(render_start) > 1.0 / 60.0 &&
          d->time - physics_start >= 1.0 / 60.0) {
        this->LookAt(&cam, d);

        // get framebuffer viewport
        mjrRect viewport = {0, 0, 0, 0};
        glfwGetFramebufferSize(window, &viewport.width, &viewport.height);

        // update scene and render
        mjv_updateScene(m, d, &opt, NULL, &cam, mjCAT_ALL, &scn);
        mjr_render(viewport, &scn, &con);

        // swap OpenGL buffers (blocking call due to v-sync)
        glfwSwapBuffers(window);

        // process pending GUI events, call GLFW callbacks
        glfwPollEvents();

        // reset render timer
        physics_start = d->time;
        render_start = std::chrono::steady_clock::now();
      }

      // wait
      double timer = GetDuration<double>(start);
      while (timer < m->opt.timestep * this->NumDecimation()) {
        timer = GetDuration<double>(start);
      }
    }

// close GLFW, free visualization storage
// terminate GLFW (crashes with Linux NVidia drivers)
#if defined(__APPLE__) || defined(_WIN32)
    glfwTerminate();
#endif
    mjv_freeScene(&scn);
    mjr_freeContext(&con);
  }

  // set look at position for visualization
  virtual void LookAt(mjvCamera* cam, const mjData* data) const {};

  int NumDecimation() const { return ndecimation_; }
  mjModel* Model() const { return model_; }
  mjData* Data() const { return data_; }
  std::tuple<std::vector<T>, std::vector<T>> ActionLimits() const {
    int nu = model_->nu;
    std::vector<T> lower;
    std::vector<T> upper;
    for (int i = 0; i < nu; i++) {
      lower.push_back(model_->actuator_ctrlrange[2 * i]);
      upper.push_back(model_->actuator_ctrlrange[2 * i + 1]);
    }
    return std::make_tuple(lower, upper);
  }

 protected:
  mjModel* model_ = nullptr;
  mjData* data_ = nullptr;
  int ndecimation_;
};

template <typename T>
class AntEnvironment : public MuJoCoEnvironment<T> {
 public:
  // default constructor
  AntEnvironment() : MuJoCoEnvironment<T>(LoadTestModel("../models/ant.xml")) {
    // overwrite defaults
    this->nobservation_ = this->model_->nq - 2 + this->model_->nv +
                          0 * (this->model_->nbody - 1) * 6;
    this->ndecimation_ = 5;
  }

  // step environment
  void Step(T* observation, T* reward, int* done, const T* action) override {
    // get previous x position
    T previous_x = this->data_->qpos[0];

    // set ctrl
    mju_copy(this->data_->ctrl, action, this->model_->nu);

    // step physics
    for (int t = 0; t < this->ndecimation_; t++) {
      mj_step(this->model_, this->data_);
    }

    // compute final forces and accelerations
    mj_rnePostConstraint(this->model_, this->data_);

    // observation
    Observation(observation);

    // get current x position
    T current_x = this->data_->qpos[0];

    // x velocity
    T dt = this->model_->opt.timestep * this->ndecimation_;
    T velocity_x = (current_x - previous_x) / dt;

    // status
    bool healthy = IsHealthy();
    done[0] = healthy ? 0 : 1;

    // -- reward -- //

    // contact penalty
    T contact_penalty = 0.0;
    for (int i = 0; i < (this->model_->nbody - 1) * 6; i++) {
      T clipped_contact = mju_clip(this->data_->cfrc_ext[6 + i], -1.0, 1.0);
      contact_penalty += clipped_contact * clipped_contact;
    }

    // total reward
    reward[0] = 1.0 * velocity_x + 1.0 * healthy -
                0.5 * mju_dot(action, action, this->model_->nu) -
                5e-4 * contact_penalty;
  }

  // reset environment
  void Reset(T* observation) override {
    // default reset
    mj_resetData(this->model_, this->data_);

    // sampling token
    absl::BitGen key;

    // position reset
    for (int i = 0; i < this->model_->nq; i++) {
      if (i >= 3 && i < 7) continue;  // skip quaternion

      // sample noisy joint position
      T qi = this->model_->qpos0[i] + absl::Uniform<T>(key, -0.1, 0.1);

      // set value
      this->data_->qpos[i] = qi;
    }

    // velocity reset
    for (int i = 0; i < this->model_->nv; i++) {
      this->data_->qvel[i] =
          mju_clip(absl::Gaussian<T>(key, 0.0, 1.0), -0.1, 0.1);
    }

    // observations
    Observation(observation);
  }

  // clone
  std::unique_ptr<Environment<T>> Clone() const override {
    return std::make_unique<AntEnvironment<T>>(*this);
  }

  void LookAt(mjvCamera* cam, const mjData* data) const override {
    cam->lookat[0] = data->qpos[0];
    cam->lookat[1] = data->qpos[1];
    cam->lookat[2] = data->qpos[2];
    cam->distance = 4.0;
  };

 private:
  void Observation(T* observation) {
    // set qpos[2:]
    int npos = this->model_->nq - 2;
    mju_copy(observation, this->data_->qpos + 2, npos);

    // set qvel
    for (int i = 0; i < this->model_->nv; i++) {
      observation[npos + i] = mju_clip(this->data_->qvel[i], -10.0, 10.0);
    }

    // contact forces: cfrc_ext
    // for (int i = 0; i < (this->model_->nbody - 1) * 6; i++) {
    //   observation[npos + this->model_->nv + i] =
    //       mju_clip(this->data_->cfrc_ext[6 + i], -1.0, 1.0);
    // }
  }

  bool IsHealthy() const {
    // warning
    bool warning = CheckWarnings(this->data_);
    if (warning) {
      std::cout << "rollout divergence" << std::endl;
    }

    // body height
    mjtNum height = this->data_->qpos[2];
    return (0.2 < height && height < 1.0) && !warning;
  }
};

// cheetah
template <typename T>
class CheetahEnvironment : public MuJoCoEnvironment<T> {
 public:
  // default constructor
  CheetahEnvironment()
      : MuJoCoEnvironment<T>(LoadTestModel("../models/cheetah.xml")) {
    // overwrite defaults
    this->nobservation_ = this->model_->nq - 1 + this->model_->nv;
    this->ndecimation_ = 5;
  }

  // step environment
  void Step(T* observation, T* reward, int* done, const T* action) override {
    // get previous x position
    T previous_x = this->data_->qpos[0];

    // set ctrl
    mju_copy(this->data_->ctrl, action, this->model_->nu);

    // step physics
    for (int t = 0; t < this->ndecimation_; t++) {
      mj_step(this->model_, this->data_);
    }

    // observation
    Observation(observation);

    // get current x position
    T current_x = this->data_->qpos[0];

    // x velocity
    T dt = this->model_->opt.timestep * this->ndecimation_;
    T velocity_x = (current_x - previous_x) / dt;

    // status
    done[0] = 0;

    // reward
    reward[0] =
        1.0 * velocity_x - 0.1 * mju_dot(action, action, this->model_->nu);
  }

  // reset environment
  void Reset(T* observation) override {
    // default reset
    mj_resetData(this->model_, this->data_);

    // sampling token
    absl::BitGen key;

    // position reset
    for (int i = 0; i < this->model_->nq; i++) {
      // sample noisy joint position
      T qi = this->data_->qpos[i] + absl::Uniform<T>(key, -0.1, 0.1);

      // if limited, clip to joint range
      if (this->model_->jnt_limited[i]) {
        qi = mju_clip(qi, this->model_->jnt_range[2 * i],
                      this->model_->jnt_range[2 * i + 1]);
      }

      // set value
      this->data_->qpos[i] = qi;
    }

    // velocity reset
    for (int i = 0; i < this->model_->nv; i++) {
      this->data_->qvel[i] +=
          mju_clip(absl::Gaussian<T>(key, 0.0, 1.0), -0.1, 0.1);
    }

    // observation
    Observation(observation);
  }

  // clone
  std::unique_ptr<Environment<T>> Clone() const override {
    return std::make_unique<CheetahEnvironment<T>>(*this);
  }

  void LookAt(mjvCamera* cam, const mjData* data) const override {
    cam->lookat[0] = data->qpos[0];
    cam->lookat[2] = data->qpos[1] + 0.5;
    cam->distance = 4.0;
  };

 private:
  void Observation(T* observation) {
    // set qpos[1:]
    int npos = this->model_->nq - 1;
    mju_copy(observation, this->data_->qpos + 1, npos);

    // set qvel
    mju_copy(observation + npos, this->data_->qvel, this->model_->nv);
  }
};

// https://github.com/Farama-Foundation/Gymnasium/blob/main/gymnasium/envs/mujoco/humanoid_v5.py
template <typename T>
class HumanoidEnvironment : public MuJoCoEnvironment<T> {
 public:
  // default constructor
  HumanoidEnvironment()
      : MuJoCoEnvironment<T>(LoadTestModel("../models/humanoid.xml")) {
    // initialize
    this->nobservation_ =
        this->model_->nq - 2 + this->model_->nv +
        (this->model_->nbody - 1) * 10 + (this->model_->nbody - 1) * 6 +
        (this->model_->nv - 6) + 0 * (this->model_->nbody - 1) * 6;
    this->ndecimation_ = 5;
  }

  // step environment
  void Step(T* observation, T* reward, int* done, const T* action) override {
    // get previous x position
    T previous_x = this->data_->qpos[0];

    // set ctrl
    mju_copy(this->data_->ctrl, action, this->model_->nu);

    // step physics
    for (int t = 0; t < this->ndecimation_; t++) {
      mj_step(this->model_, this->data_);
    }

    // compute final forces and accelerations
    mj_rnePostConstraint(this->model_, this->data_);

    // observation
    Observation(observation);

    // get current x position
    T current_x = this->data_->qpos[0];

    // x velocity
    T dt = this->model_->opt.timestep * this->ndecimation_;
    T velocity_x = (current_x - previous_x) / dt;

    // status
    bool healthy = IsHealthy();
    done[0] = healthy ? 0 : 1;

    // -- reward -- //

    // contact penalty
    T contact_penalty =
        mju_dot(this->data_->cfrc_ext + 6, this->data_->cfrc_ext + 6,
                (this->model_->nbody - 1) * 6);
    contact_penalty =
        mju_clip(5.0e-7 * contact_penalty, -1.0e8, 10.0);  // [-inf, 10]
    // T contact_penalty = 0.0;

    // total reward
    reward[0] = 1.25 * velocity_x + 5.0 * healthy -
                0.1 * mju_dot(action, action, this->model_->nu) -
                contact_penalty;
  }

  // reset environment
  void Reset(T* observation) override {
    // default reset
    mj_resetData(this->model_, this->data_);

    // sampling token
    absl::BitGen key;

    // position reset
    for (int i = 0; i < this->model_->nq; i++) {
      // sample noisy joint position
      T qi = this->model_->qpos0[i] + 1.0e-2 * absl::Uniform<T>(key, -1.0, 1.0);

      // set value
      this->data_->qpos[i] = qi;
    }

    // velocity reset
    for (int i = 0; i < this->model_->nv; i++) {
      this->data_->qvel[i] = 1.0e-2 * absl::Uniform<T>(key, -1.0, 1.0);
    }

    // observation
    Observation(observation);
  }

  // clone
  std::unique_ptr<Environment<T>> Clone() const override {
    return std::make_unique<HumanoidEnvironment<T>>(*this);
  }

  void LookAt(mjvCamera* cam, const mjData* data) const override {
    cam->lookat[0] = data->qpos[0];
    cam->lookat[1] = data->qpos[1];
    cam->lookat[2] = data->qpos[2];
    cam->distance = 4.0;
  };

 private:
  void Observation(T* observation) {
    // set qpos[2:]
    int npos = this->model_->nq - 2;
    mju_copy(observation, this->data_->qpos + 2, npos);

    // set qvel
    int nv = this->model_->nv;
    mju_copy(observation + npos, this->data_->qvel, nv);

    // set com inertia
    int ncominertia = (this->model_->nbody - 1) * 10;
    T* com_inertia = this->data_->cinert + 10;
    mju_copy(observation + npos + nv, com_inertia, ncominertia);

    // set com velocity
    int ncomvelocity = (this->model_->nbody - 1) * 6;
    T* com_velocity = this->data_->cvel + 6;
    mju_copy(observation + npos + nv + ncominertia, com_velocity, ncomvelocity);

    // set actuator forces
    int nactuatorforces = this->model_->nv - 6;
    T* actuator_forces = this->data_->qfrc_actuator + 6;
    mju_copy(observation + npos + nv + ncominertia + ncomvelocity,
             actuator_forces, nactuatorforces);

    // set contact_forces
    // int ncontactforces = (this->model_->nbody - 1) * 6;
    // T* contact_forces = this->data_->cfrc_ext + 6;
    // mju_copy(observation + npos + nv + ncominertia + ncomvelocity +
    //              nactuatorforces,
    //          contact_forces, ncontactforces);
  }

  bool IsHealthy() const {
    // warning
    bool warning = CheckWarnings(this->data_);
    if (warning) {
      std::cout << "rollout divergence" << std::endl;
    }

    // body height
    mjtNum height = this->data_->qpos[2];

    return (1.0 < height && height < 2.0) && !warning;
  }
};

template <typename T>
class WalkerEnvironment : public MuJoCoEnvironment<T> {
 public:
  // default constructor
  WalkerEnvironment()
      : MuJoCoEnvironment<T>(LoadTestModel("../models/walker.xml")) {
    // overwrite defaults
    this->nobservation_ = this->model_->nq - 1 + this->model_->nv;
    this->ndecimation_ = 4;
  }

  // step environment
  void Step(T* observation, T* reward, int* done, const T* action) override {
    // get previous x position
    T previous_x = this->data_->qpos[0];

    // set ctrl
    mju_copy(this->data_->ctrl, action, this->model_->nu);

    // step physics
    for (int t = 0; t < this->ndecimation_; t++) {
      mj_step(this->model_, this->data_);
    }

    // observation
    Observation(observation);

    // get current x position
    T current_x = this->data_->qpos[0];

    // x velocity
    T dt = this->model_->opt.timestep * this->ndecimation_;
    T velocity_x = (current_x - previous_x) / dt;

    // status
    bool healthy = IsHealthy();
    done[0] = healthy ? 0 : 1;

    // reward
    reward[0] = 1.0 * velocity_x + 1.0 * healthy -
                0.001 * mju_dot(action, action, this->model_->nu);
  }

  // reset environment
  void Reset(T* observation) override {
    // default reset
    mj_resetData(this->model_, this->data_);

    // sampling token
    absl::BitGen key;

    // position reset
    for (int i = 0; i < this->model_->nq; i++) {
      // sample noisy joint position
      T qi = this->model_->qpos0[i] + 5.0e-3 * absl::Uniform<T>(key, -1.0, 1.0);

      // set value
      this->data_->qpos[i] = qi;
    }

    // velocity reset
    for (int i = 0; i < this->model_->nv; i++) {
      this->data_->qvel[i] = 5.0e-3 * absl::Uniform<T>(key, -1.0, 1.0);
    }

    // observation
    Observation(observation);
  }

  // clone
  std::unique_ptr<Environment<T>> Clone() const override {
    return std::make_unique<WalkerEnvironment<T>>(*this);
  }

  void LookAt(mjvCamera* cam, const mjData* data) const override {
    cam->lookat[0] = data->qpos[0];
    cam->lookat[2] = data->qpos[1];
    cam->distance = 4.0;
  };

 private:
  void Observation(T* observation) {
    // set qpos[2:]
    int npos = this->model_->nq - 1;
    mju_copy(observation, this->data_->qpos + 1, npos);

    // set qvel
    mju_copy(observation + npos, this->data_->qvel, this->model_->nv);
    for (int i = 0; i < this->model_->nv; i++) {
      observation[npos + i] = mju_clip(observation[npos + i], -10.0, 10.0);
    }
  }

  bool IsHealthy() const {
    // warning
    bool warning = CheckWarnings(this->data_);
    if (warning) {
      std::cout << "rollout divergence" << std::endl;
    }

    // body height
    mjtNum height = this->data_->qpos[1];

    // body angle
    mjtNum angle = this->data_->qpos[2];
    return ((0.8 < height && height < 2.0) && (-1.0 < angle && angle < 1.0)) &&
           !warning;
  }
};

// -- search -- //

// (default) settings for random search
struct Settings {
  int nsample = 128;
  int ntop = 64;
  int niter = 100;
  int neval = 10;
  int nhorizon_search = 1000;
  int nhorizon_eval = 1000;
  mjtNum random_step = 0.01;
  mjtNum update_step = 0.01;
  mjtNum reward_shift = 0.0;
  int num_threads = 20;
  int nenveval = 128;
  bool visualize = true;
};

// search
template <typename T>
class Search {
 public:
  // constructor
  Search(Environment<T>& env, Policy<T>& policy, Settings settings)
      : observation_statistics_(env.NumObservation()),
        rewards_best_statistics_(1),
        step_statistics_(1),
        rewards_eval_statistics_(1),
        pool_(settings.num_threads) {
    // start timer
    auto start = std::chrono::steady_clock::now();

    // initialize
    settings_ = settings;

    // policy
    policy_ = &policy;

    // data
    std::vector<T> perturb(policy_->NumWeight());
    for (int i = 0; i < 2 * settings_.nsample; i++) {
      // copy environment
      env_.emplace_back(env.Clone());

      // rollout policies (positive and negative policies)
      rollout_policy_.push_back(*policy_);

      // statistics
      rollout_statistics_.push_back(observation_statistics_);

      // policy perturbation
      if (i >= settings_.nsample) continue;
      perturb_.push_back(perturb);
    }

    rewards_.resize(2 * settings_.nsample);
    rewards_eval_.resize(settings_.nenveval);
    weight_update_.resize(policy_->NumWeight());

    // add additional objects for evaluation
    for (int i = 0; i < settings_.nenveval - 2 * settings_.nsample; i++) {
      // copy environment
      env_.emplace_back(env.Clone());

      // policy
      rollout_policy_.push_back(*policy_);

      // observation statistics
      rollout_statistics_.push_back(observation_statistics_);
    }

    T gen_time = GetDuration<T>(start);
  }

  void PolicyPerturbation() {
    // sample policy perturbations (parallel)
    int count_before = pool_.GetCount();
    for (int i = 0; i < settings_.nsample; i++) {
      pool_.Schedule([&s = *this, i]() {
        // sampling token
        absl::BitGen key;

        // sample noise elements
        for (int j = 0; j < s.policy_->NumWeight(); j++) {
          s.perturb_[i][j] = absl::Gaussian(key, 0.0, 1.0);
        }

        // -- copy nominal policy -- //

        // positive perturbation policy
        s.rollout_policy_[i].Update(s.policy_->Weight(), s.policy_->Shift(),
                                    s.policy_->Scale());

        // negative perturbation policy
        s.rollout_policy_[s.settings_.nsample + i].Update(
            s.policy_->Weight(), s.policy_->Shift(), s.policy_->Scale());

        // perturb positive policy weights
        s.rollout_policy_[i].PerturbWeights(s.perturb_[i],
                                            1.0 * s.settings_.random_step);

        s.rollout_policy_[s.settings_.nsample + i].PerturbWeights(
            s.perturb_[i], -1.0 * s.settings_.random_step);
      });
    }
    pool_.WaitCount(count_before + settings_.nsample);
    pool_.ResetCount();
  }

  void RandomRollouts() {
    // rollouts (parallel) :star:
    int count_before = pool_.GetCount();
    for (int i = 0; i < 2 * settings_.nsample; i++) {
      pool_.Schedule([&s = *this, i]() {
        // environment + policy
        Environment<T>* env = s.env_[i].get();
        Policy<T>* policy = &s.rollout_policy_[i];

        // reset observation statistics
        s.rollout_statistics_[i].Reset();

        // initialize
        T total_reward = 0.0;
        std::vector<T> reward = {0.0};
        std::vector<int> done = {0};
        std::vector<T> action(env->NumAction());
        std::vector<T> observation(env->NumObservation());

        // get observation
        env->Reset(observation.data());
        s.rollout_statistics_[i].Update(observation);

        // simulate environment
        for (int t = 0; t < s.settings_.nhorizon_search; t++) {
          // action from policy
          policy->Evaluate(action, observation);

          // step environment
          env->Step(observation.data(), reward.data(), done.data(),
                    action.data());
          reward[0] -= s.settings_.reward_shift;

          // check done
          if (done[0] == 1) break;

          // get next observation
          s.rollout_statistics_[i].Update(observation);

          // update total reward
          total_reward += reward[0];
        }

        s.rewards_[i] = total_reward;
      });
    }
    pool_.WaitCount(count_before + 2 * settings_.nsample);
    pool_.ResetCount();
  }

  void Evaluation() {
    int count_before = pool_.GetCount();
    for (int i = 0; i < settings_.nenveval; i++) {
      pool_.Schedule([&s = *this, i]() {
        // copy nominal policy
        s.rollout_policy_[i].Update(s.policy_->Weight(), s.policy_->Shift(),
                                    s.policy_->Scale());

        // reset observation statistics
        s.rollout_statistics_[i].Reset();

        // environment + policy
        Environment<T>* env = s.env_[i].get();
        Policy<T>* policy = &s.rollout_policy_[i];

        // initialize
        T total_reward = 0.0;
        std::vector<T> reward = {0.0};
        std::vector<int> done = {0};
        std::vector<T> action(env->NumAction());
        std::vector<T> observation(env->NumObservation());

        // get observation
        env->Reset(observation.data());
        s.rollout_statistics_[i].Update(observation);

        // simulate environment
        for (int t = 0; t < s.settings_.nhorizon_eval; t++) {
          // action from policy
          policy->Evaluate(action, observation);

          // step environment
          env->Step(observation.data(), reward.data(), done.data(),
                    action.data());

          // check done
          if (done[0] == 1) break;

          // get next observation
          s.rollout_statistics_[i].Update(observation);

          // update total reward
          total_reward += reward[0];
        }

        s.rewards_eval_[i] = total_reward;
      });
    }
    pool_.WaitCount(count_before + settings_.nenveval);
    pool_.ResetCount();

    // compute reward statistics
    rewards_eval_statistics_.Reset();
    step_statistics_.Reset();
    for (int i = 0; i < settings_.nenveval; i++) {
      rewards_eval_statistics_.Update(rewards_eval_[i]);
      step_statistics_.Update(rollout_statistics_[i].Count());
    }
  }

  int Iteration() {
    // each search iteration

    // sample random policies
    PolicyPerturbation();

    // simulate environments to collect rewards
    RandomRollouts();

    // combine observation statistics (sequential)
    int env_steps = 0;
    for (int i = 0; i < 2 * settings_.nsample; i++) {
      env_steps += rollout_statistics_[i].Count() - 1;
      observation_statistics_.Merge(rollout_statistics_[i]);
    }

    // -- rollout order -- //

    // initialize
    rollout_order_.clear();
    for (int i = 0; i < settings_.nsample; i++) {
      rollout_order_.push_back(i);
    }

    // sort (decreasing)
    // TODO(taylor): just sort ntop
    std::sort(rollout_order_.begin(), rollout_order_.end(),
              [rewards = rewards_, nsample = settings_.nsample](int a, int b) {
                return std::max(rewards[a], rewards[nsample + a]) >
                       std::max(rewards[b], rewards[nsample + b]);
              });

    // -- policy weight update -- //

    // reset update
    std::fill(weight_update_.begin(), weight_update_.end(), 0.0);

    // reset statistics
    rewards_best_statistics_.Reset();
    step_statistics_.Reset();

    // loop over best rewards to compute update
    for (int i = 0; i < settings_.ntop; i++) {
      // index
      int idx = rollout_order_[i];

      // steps
      step_statistics_.Update(T(rollout_statistics_[idx].Count()));

      // rewards
      T reward_positive = rewards_[idx];
      T reward_negative = rewards_[settings_.nsample + idx];

      // update statistics
      rewards_best_statistics_.Update(reward_positive);
      rewards_best_statistics_.Update(reward_negative);

      // update
      mju_addToScl(weight_update_.data(), perturb_[idx].data(),
                   reward_positive - reward_negative, policy_->NumWeight());
    }

    // update weights
    T update_rate = settings_.update_step / settings_.ntop /
                    (rewards_best_statistics_.StandardDeviation()[0] + 1.0e-5);
    policy_->PerturbWeights(weight_update_, update_rate);

    // update policy
    policy_->Update(policy_->Weight(), observation_statistics_.Mean(),
                    observation_statistics_.StandardDeviation());

    return env_steps;
  }

  void Run(std::string filename = std::string()) {
    // main funtion to search for the best policy

    // start timer
    auto start_search = std::chrono::steady_clock::now();

    // initialize observation statistics
    observation_statistics_.InitializeIdentity();

    // evaluation iterations
    int iter_per_eval = int(settings_.niter / settings_.neval);
    for (int i = 0; i < settings_.neval; i++) {
      // start timer
      auto start_iteration = std::chrono::steady_clock::now();

      // search
      int env_steps = 0;
      for (int j = 0; j < iter_per_eval; j++) {
        env_steps += Iteration();
      }

      // stop timer
      T iteration_time = GetDuration<T>(start_iteration);

      // evaluate
      Evaluation();

      // reward statistics
      int avg_steps = int(step_statistics_.Mean()[0] - 1.0);
      std::cout << "iteration (" << (i + 1) * iter_per_eval << "/"
                << settings_.niter
                << "): reward = " << rewards_eval_statistics_.Mean()[0]
                << " +- " << rewards_eval_statistics_.StandardDeviation()[0]
                << " | time: " << iteration_time
                << " | episode length: " << avg_steps << " +- "
                << int(step_statistics_.StandardDeviation()[0])
                << " | global steps: " << observation_statistics_.Count()
                << " | steps / second: " << int(env_steps / iteration_time)
                << std::endl;

      // checkpoint policy
      if (!filename.empty()) {
        std::string checkpoint =
            filename + "_" + std::to_string(i) + "_" +
            std::to_string(int(rewards_eval_statistics_.Mean()[0])) + "_" +
            std::to_string(
                int(rewards_eval_statistics_.StandardDeviation()[0]));
        policy_->Save(checkpoint);
      }

      // // visualize policy
      // if (settings_.visualize && (i % 1 == 0 && i > 0))
      //   env_[0].get()->Visualize(*policy_, avg_steps);
    }

    // total search time
    T search_time = GetDuration<T>(start_search);
    std::cout << "total time: " << search_time << std::endl;
  }

 private:
  // environment
  std::vector<std::unique_ptr<Environment<T>>> env_;

  // policy
  Policy<T>* policy_;

  // settings
  Settings settings_;

  // data
  std::vector<Policy<T>> rollout_policy_;
  std::vector<std::vector<T>> perturb_;
  RunningStatistics<T> observation_statistics_;
  std::vector<RunningStatistics<T>> rollout_statistics_;
  std::vector<T> rewards_;
  std::vector<T> rewards_eval_;
  std::vector<T> weight_update_;
  std::vector<int> rollout_order_;
  RunningStatistics<T> rewards_best_statistics_;
  RunningStatistics<T> step_statistics_;
  RunningStatistics<T> rewards_eval_statistics_;

  // threadpool
  ThreadPool pool_;
};

// parse command-line arguments (from Claude Sonnet 3.5)
class ArgumentParser {
 private:
  std::unordered_map<std::string, std::string> args;

 public:
  ArgumentParser(int argc, char* argv[]) {
    for (int i = 1; i < argc; i++) {
      std::string arg = argv[i];
      if (arg.substr(0, 2) == "--") {
        std::string key = arg.substr(2);
        if (i + 1 < argc && argv[i + 1][0] != '-') {
          args[key] = argv[++i];
        } else {
          args[key] = "true";
        }
      }
    }
  }

  // get value based on key; if no key, return default value
  std::string Get(const std::string& key,
                  const std::string& default_value = "") {
    return args.count(key) ? args[key] : default_value;
  }

  // check for key
  bool Has(const std::string& key) { return args.count(key) > 0; }
};

// return environment
MuJoCoEnvironment<mjtNum>* GetEnvironment(const std::string str) {
  if (str == "ant") return new AntEnvironment<mjtNum>;
  if (str == "cheetah") return new CheetahEnvironment<mjtNum>;
  if (str == "humanoid") return new HumanoidEnvironment<mjtNum>;
  if (str == "walker") return new WalkerEnvironment<mjtNum>;
  return NULL;  // default
}

// run random search
int main(int argc, char* argv[]) {
  // parse settings from command line
  ArgumentParser parser(argc, argv);

  if (parser.Has("help")) {
    std::cout << "Usage: " << argv[0] << " [options]" << std::endl;
    std::cout << "Setup:" << std::endl;
    std::cout << "  --env: ant, cheetah, humanoid, walker" << std::endl;
    std::cout << "  --search" << std::endl;
    std::cout << "  --checkpoint: <filename in checkpoint directory>"
              << std::endl;
    std::cout << "  --load: <filename in checkpoint directory>" << std::endl;
    std::cout << "  --visualize" << std::endl;
    std::cout << "Search settings:" << std::endl;
    std::cout << "  --nsample: (int) number of random directions to sample"
              << std::endl;
    std::cout << "  --ntop: (int) number of random directions to use for "
                 "policy update"
              << std::endl;
    std::cout << "  --niter: (int) number of policy updates" << std::endl;
    std::cout << "  --neval: (int) number of policy evaluations during search"
              << std::endl;
    std::cout << "  --nhorizon_search: (int) number of environment steps "
                 "during policy improvement"
              << std::endl;
    std::cout << "  --nhorizon_eval: (int) number of environment steps during "
                 "policy evaluation"
              << std::endl;
    std::cout << "  --random_step: (float) step size for random direction "
                 "during policy perturbation"
              << std::endl;
    std::cout << "  --update_step: (float) step size for policy update during "
                 "policy improvement"
              << std::endl;
    std::cout
        << "  --nenveval: (int) number of environments for policy evaluation"
        << std::endl;
    std::cout << "  --reward_shift: (float) subtract baseline from "
                 "per-timestep reward"
              << std::endl;
    std::cout << "  --num_threads: (int) number of threads (workers)"
              << std::endl;
    return 0;
  }

  // environment
  std::string env_str = parser.Get("env", "");
  MuJoCoEnvironment<mjtNum>* env = GetEnvironment(env_str);

  // require valid environment
  if (!env) {
    std::cout << "Environment not specified" << std::endl;
    return 1;
  }

  // policy
  auto action_limits = env->ActionLimits();
  Policy p = Policy<mjtNum>(env->NumAction(), env->NumObservation(),
                            get<0>(action_limits), get<1>(action_limits));

  // load policy
  if (parser.Has("load")) {
    // checkpoint path
    std::filesystem::path cwd = std::filesystem::current_path();
    std::string checkpoint_dir = cwd.string() + "/../checkpoint";
    std::string checkpoint_path = checkpoint_dir + "/" + parser.Get("load");

    // load policy
    p.Load(checkpoint_path);
    std::cout << "Successfully loaded checkpoint: " << checkpoint_path
              << std::endl;
  }

  // default settings
  Settings settings;

  // ant
  if (env_str == "ant") {
    settings.nsample = 60;
    settings.ntop = 20;
    settings.niter = 1000;
    settings.neval = 10;
    settings.random_step = 0.025;
    settings.update_step = 0.015;
    settings.reward_shift = 1.0;
    // cheetah
  } else if (env_str == "cheetah") {
    settings.nsample = 32;
    settings.ntop = 4;
    settings.niter = 100;
    settings.neval = 10;
    settings.random_step = 0.03;
    settings.update_step = 0.02;
    settings.reward_shift = 0.0;
    // humanoid
  } else if (env_str == "humanoid") {
    settings.nsample = 320;
    settings.ntop = 320;
    settings.niter = 1000;
    settings.neval = 10;
    settings.random_step = 0.0075;
    settings.update_step = 0.02;
    settings.reward_shift = 5.0;
    // walker
  } else if (env_str == "walker") {
    settings.nsample = 40;
    settings.ntop = 30;
    settings.niter = 1000;
    settings.neval = 10;
    settings.random_step = 0.025;
    settings.update_step = 0.03;
    settings.reward_shift = 1.0;
  }

  // update settings with command line arguments
  std::vector<std::string> keys = {
      "nsample",         "ntop",          "niter",       "neval",
      "nhorizon_search", "nhorizon_eval", "random_step", "update_step",
      "nenveval",        "reward_shift",  "num_threads"};

  for (auto key : keys) {
    if (parser.Has(key)) {
      if (key == "nsample") {
        settings.nsample = std::stoi(parser.Get(key));
      } else if (key == "ntop") {
        settings.ntop = std::stoi(parser.Get(key));
      } else if (key == "niter") {
        settings.niter = std::stoi(parser.Get(key));
      } else if (key == "neval") {
        settings.neval = std::stoi(parser.Get(key));
      } else if (key == "nhorizon_search") {
        settings.nhorizon_search = std::stoi(parser.Get(key));
      } else if (key == "nhorizon_eval") {
        settings.nhorizon_eval = std::stoi(parser.Get(key));
      } else if (key == "random_step") {
        settings.random_step = std::stod(parser.Get(key));
      } else if (key == "update_step") {
        settings.update_step = std::stod(parser.Get(key));
      } else if (key == "nenveval") {
        settings.nenveval = std::stoi(parser.Get(key));
      } else if (key == "reward_shift") {
        settings.reward_shift = std::stod(parser.Get(key));
      } else if (key == "num_threads") {
        settings.num_threads = std::stoi(parser.Get(key));
      }
    }
  }

  // run random search
  if (parser.Has("search")) {
    // print search settings
    std::cout << "Settings: " << std::endl;
    std::cout << "  environment: " << parser.Get("env") << std::endl;
    std::cout << "  nsample: " << settings.nsample
              << " | ntop: " << settings.ntop << std::endl;
    std::cout << "  niter: " << settings.niter << " | neval: " << settings.neval
              << std::endl;
    std::cout << "  nhorizon_search: " << settings.nhorizon_search
              << " | nhorizon_eval: " << settings.nhorizon_eval << std::endl;
    std::cout << "  random_step: " << settings.random_step
              << " | update_step: " << settings.update_step << std::endl;
    std::cout << "  nenveval: " << settings.nenveval << std::endl;
    std::cout << "  reward_shift: " << settings.reward_shift << std::endl;
    std::cout << "  num_threads: " << settings.num_threads << std::endl;

    // search
    Search<mjtNum> search(*env, p, settings);

    // create checkpoint directory
    std::filesystem::path cwd = std::filesystem::current_path();
    std::string checkpoint_dir = cwd.string() + "/../checkpoint";

    if (!std::filesystem::is_directory(checkpoint_dir) ||
        !std::filesystem::exists(checkpoint_dir)) {
      std::filesystem::create_directory(checkpoint_dir);
    }

    // checkpoint path
    std::string checkpoint_path;
    if (parser.Has("checkpoint")) {
      checkpoint_path = checkpoint_dir + "/" + parser.Get("checkpoint");
    }

    // run
    search.Run(checkpoint_path);
  }

  // visualize
  if (parser.Has("visualize")) {
    env->Visualize(p, settings.nhorizon_eval);
  }

  return 0;
}
