// bnats2.cpp : �R���\�[�� �A�v���P�[�V�����̃G���g�� �|�C���g���`���܂��B
#include <math.h>

#include <cstdio>
#include <string>

#include "culc.h"
#include "disp.h"

using namespace std;

int count = 0;

const int SHOW_FOR_EACH = 30;

// void mouse(int button, int state, int x, int y) {
//   switch (button) {
//     case GLUT_LEFT_BUTTON:
//       if (state == GLUT_DOWN) {
//         /*�A�j���[�V�����J�n*/
//         glutIdleFunc(idle);
//       }
//       break;
//     case GLUT_RIGHT_BUTTON:
//       if (state == GLUT_DOWN) {
//         /* �A�j���[�V������~*/
//         glutIdleFunc(0);
//       }
//       break;
//     default:
//       break;
//   }
// }

// void keyboard(unsigned char key, int x, int y) {
//   switch (key) {
//     case 'q':
//     case 'Q':
//     case 'e':
//     case 'E':
//     case '\033':
//       exit(0);
//     default:
//       break;
//   }
// }

void init(Xsdata *xdata, string window_name) {
  datainit(xdata);
  cv::namedWindow(window_name);
}

void update(Xsdata *xdata, string window_name) {
  static double time = 0;

  for (int i = 0; i < SHOW_FOR_EACH; i++) {
    // ���̃A�b�v�f�[�g
    time += dt;
    if (time > 10 / nyu) time = 0;
    base = A * pow(sin(2 * M_PI * nyu * time), 1);

    // base2 = base*B / A;
    interaction(xdata);
    partupdate(xdata);
  }
  auto img = udisp(xdata, num_particles2, height, width, dcell);
  cv::imshow(window_name, img);
}

int main(int argc, char *argv[]) {
  Xsdata *xdata = new Xsdata[num_particles];
  string window_name = "";
  init(xdata, window_name);

  int key;
  double time = 0;
  while (1) {
    int key;
    update(xdata, window_name);
    key = cv::waitKey(1);
    // 'Esc'�������ꂽ�ꍇ�ɏI��
    if (key == 27) break;
  }
	cv::destroyWindow(window_name);

  delete[] xdata;
  return 0;
}
