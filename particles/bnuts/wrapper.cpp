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

class BNutsEnvPy : public BNutsEnv {
  py::array get_image_nparray() { return mat_to_nparray(get_image()); }
};

PYBIND11_MODULE(bnutsenv, m) {
  m.doc() = "bnats env module";
  // m.def("add", &add, "add two numbers");
}