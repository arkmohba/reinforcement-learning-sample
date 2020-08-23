#include "b_nuts_env.h"

using namespace std;

const int SHOW_FOR_EACH = 30;

int main(int argc, char *argv[]) {
  BNutsEnv* env;
  try {
    env = new BNutsEnv();
  } catch (exception e) {
    cout << e.what() << endl;
    delete env;
    return 1;
  }
  try {
  env->init();
  } catch (exception e) {
    cout << e.what() << endl;
    delete env;
    return 1;
  }

  while (true) {
    for (int i = 0; i < SHOW_FOR_EACH; i++) {
      env->update();
    }
    env->show_image();
    int key = cv::waitKey(1);
    // 'Esc'が押された場合に終了
    if (key == 27) break;
  }

  return 0;
}
