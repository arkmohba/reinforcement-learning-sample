#include <algorithm>

#include "b_nuts_env.h"
#include "pybind11/numpy.h"
#include "pybind11/pybind11.h"

namespace py = pybind11;

/* 参考：
https://stackoverflow.com/questions/60949451/how-to-send-a-cvmat-to-python-over-shared-memory
 */

py::dtype determine_np_dtype(int depth) {
  switch (depth) {
    case CV_8U:
      return py::dtype::of<uint8_t>();
    case CV_8S:
      return py::dtype::of<int8_t>();
    case CV_16U:
      return py::dtype::of<uint16_t>();
    case CV_16S:
      return py::dtype::of<int16_t>();
    case CV_32S:
      return py::dtype::of<int32_t>();
    case CV_32F:
      return py::dtype::of<float>();
    case CV_64F:
      return py::dtype::of<double>();
    default:
      throw std::invalid_argument("Unsupported data type.");
  }
}

std::vector<std::size_t> determine_shape(cv::Mat& m) {
  if (m.channels() == 1) {
    return {static_cast<size_t>(m.rows), static_cast<size_t>(m.cols)};
  }

  return {static_cast<size_t>(m.rows), static_cast<size_t>(m.cols),
          static_cast<size_t>(m.channels())};
}

py::capsule make_capsule(cv::Mat& m) {
  return py::capsule(new cv::Mat(m),
                     [](void* v) { delete reinterpret_cast<cv::Mat*>(v); });
}

/***
 * OpenCVからNumpyに変換
 ***/
py::array mat_to_nparray(cv::Mat& m) {
  if (!m.isContinuous()) {
    throw std::invalid_argument("Only continuous Mats supported.");
  }

  return py::array(determine_np_dtype(m.depth()), determine_shape(m), m.data,
                   make_capsule(m));
}

const Xsdata* min_y(const Xsdata* a, const Xsdata* b) {
  return (a->y < b->y) ? a : b;
}

class BNutsEnvPy : public BNutsEnv {
 public:
  void init();
  void update_with_base(double base);
  py::array get_image_array() { return mat_to_nparray(get_image()); }
  double get_bnuts_y() { return xdata[0].y; }
  double get_others_min_y() {
    return std::min(xdata, xdata + num_particles, min_y)->y;
  }
  int get_width() { return width; }
  int get_height() { return height; }
  int get_channels() { return 3; }
  int get_max_diff() { return A; }
};

void BNutsEnvPy::init() {
  datainit(xdata, 0, 0, l_list);
  for (int i = 0; i < WARMUP_START_UP; i++) {
    interaction(xdata, 0, 0, l_list);
    partupdate(xdata);
  }
}

void BNutsEnvPy::update_with_base(double base) {
  interaction(xdata, base, 0, l_list);
  partupdate(xdata);
}

PYBIND11_MODULE(bnutsenv, m) {
  m.doc() = "bnats env module";
  py::class_<BNutsEnvPy>(m, "BNutsEnvCore")
      .def(py::init<>())
      .def("update_with_base", &BNutsEnvPy::update_with_base)
      .def("init", &BNutsEnvPy::init)
      .def("get_image_array", &BNutsEnvPy::get_image_array)
      .def("show_image", &BNutsEnvPy::show_image)
      .def("get_bnuts_y", &BNutsEnvPy::get_bnuts_y)
      .def("get_others_min_y", &BNutsEnvPy::get_others_min_y)
      .def("get_width", &BNutsEnvPy::get_width)
      .def("get_height", &BNutsEnvPy::get_height)
      .def("get_channels", &BNutsEnvPy::get_channels)
      .def("get_max_diff", &BNutsEnvPy::get_max_diff);
}