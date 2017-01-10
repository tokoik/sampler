#include <cstdlib>
#include <cmath>
#include <iostream>
#include <vector>
#include "gg.h"
#include "Window.h"

// ��������_�̐�
const GLint points(10000);

// ��l�������� (Xorshift �@)
float xorshift()
{
  static uint32_t y(2463534242);
  
  // �f�[�^�̓���ւ�
  y = y ^ (y << 13);
  y = y ^ (y >> 17);
  
  // [0, 1] �ɐ��K��
  return static_cast<float>(y = y ^ (y << 5)) * 2.3283064e-10f;
}

// �T���v���_�̍쐬
void createSampler(GLsizei samples, std::vector<GLfloat> &sample, GLfloat n)
{
  // e �� 1 / (n + 1)
  const auto e(1.0f / (n + 1.0f));
  
  for (unsigned int i = 0; i < samples; ++i)
  {
    const auto z(pow(xorshift(), e));
    const auto d(sqrt(1.0f - z * z));
    const auto t(6.2831853f * xorshift());
    const auto x(d * cos(t));
    const auto y(d * sin(t));
    
    sample.push_back(y);
    sample.push_back(z);
    sample.push_back(x);
    //sample.push_back(pow(xorshift(), 1.0f / 3.0f));
    sample.push_back(pow(z, n));
    //sample.push_back(1.0f);
  }
}

//
// ���C��
//
int main()
{
  // GLFW ������������
  if (glfwInit() == GL_FALSE)
  {
    // �������Ɏ��s����
    std::cerr << "Can't initialize GLFW.\n";
    return EXIT_FAILURE;
  }
  
  // �v���O�����I�����̏�����o�^����
  atexit(glfwTerminate);
  
  // OpenGL Version 3.2 Core Profile ��I������
  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 2);
  glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
  glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
  
  // �E�B���h�E���쐬����
  Window window(300, 300);
  
  // �E�B���h�E���J�������ǂ����m���߂�
  if (!window.get())
  {
    // �E�B���h�E���J���Ȃ�����
    std::cerr << "Can't open GLFW window.\n";
    return EXIT_FAILURE;
  }
  
  // �_�Q���쐬����
  std::vector<GLfloat> sample;
  createSampler(points, sample, 10.0f);
  GLuint vao;
  glGenVertexArrays(1, &vao);
  glBindVertexArray(vao);
  GLuint vbo;
  glGenBuffers(1, &vbo);
  glBindBuffer(GL_ARRAY_BUFFER, vbo);
  glBufferData(GL_ARRAY_BUFFER, sample.size() * sizeof (GLfloat), sample.data(), GL_STATIC_DRAW);
  glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 0, 0);
  glEnableVertexAttribArray(0);
  
  // �}�`�̕\���ɗp����V�F�[�_��ǂݍ���
  const GgPointShader point("point.vert", "point.frag");
  
  // �}�`�\���p�̎���ϊ��s���ݒ肷��
  const auto mv(ggLookat(0.0f, 0.5f, 2.5f, 0.0f, 0.5f, 0.0f, 0.0f, 1.0f, 0.0f));
  
  // �B�ʏ������s��
  glEnable(GL_DEPTH_TEST);
  glEnable(GL_CULL_FACE);

  // �E�B���h�E���J���Ă���ԌJ��Ԃ�
  while (!window.shouldClose())
  {
    // �r���[�|�[�g��ݒ肷��
    window.setViewport();
    
    // �J���[�o�b�t�@�ƃf�v�X�o�b�t�@����������
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    
    // �}�`�`��p�̃V�F�[�_�v���O�����̎g�p���J�n����
    point.use();
    
    // �}�`�\���p�̓��e�ϊ��s������݂̃E�B���h�E�̃A�X�y�N�g�ɍ��킹�Đݒ肷��
    const auto mp(ggPerspective(1.0f, window.getAspect(), 1.0f, 5.0f));

    // �ϊ��s���ݒ肷��
    point.loadMatrix(mp, mv * window.getLeftTrackball());
    
    // �}�`��`�悷��
    glBindVertexArray(vao);
    glDrawArrays(GL_POINTS, 0, points);
    
    // �J���[�o�b�t�@�����ւ��ăC�x���g�����o��
    window.swapBuffers();
  }
}
