#include <cstdlib>
#include <cmath>
#include <iostream>
#include <vector>
#include "gg.h"
#include "Window.h"

// 生成する点の数
const GLint points(10000);

// 一様乱数発生 (Xorshift 法)
float xorshift()
{
  static uint32_t y(2463534242);
  
  // データの入れ替え
  y = y ^ (y << 13);
  y = y ^ (y >> 17);
  
  // [0, 1] に正規化
  return static_cast<float>(y = y ^ (y << 5)) * 2.3283064e-10f;
}

// サンプル点の作成
void createSampler(GLsizei samples, std::vector<GLfloat> &sample, GLfloat n)
{
  // e ← 1 / (n + 1)
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
// メイン
//
int main()
{
  // GLFW を初期化する
  if (glfwInit() == GL_FALSE)
  {
    // 初期化に失敗した
    std::cerr << "Can't initialize GLFW.\n";
    return EXIT_FAILURE;
  }
  
  // プログラム終了時の処理を登録する
  atexit(glfwTerminate);
  
  // OpenGL Version 3.2 Core Profile を選択する
  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 2);
  glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
  glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
  
  // ウィンドウを作成する
  Window window(300, 300);
  
  // ウィンドウが開けたかどうか確かめる
  if (!window.get())
  {
    // ウィンドウが開けなかった
    std::cerr << "Can't open GLFW window.\n";
    return EXIT_FAILURE;
  }
  
  // 点群を作成する
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
  
  // 図形の表示に用いるシェーダを読み込む
  const GgPointShader point("point.vert", "point.frag");
  
  // 図形表示用の視野変換行列を設定する
  const auto mv(ggLookat(0.0f, 0.5f, 2.5f, 0.0f, 0.5f, 0.0f, 0.0f, 1.0f, 0.0f));
  
  // 隠面消去を行う
  glEnable(GL_DEPTH_TEST);
  glEnable(GL_CULL_FACE);

  // ウィンドウが開いている間繰り返す
  while (!window.shouldClose())
  {
    // ビューポートを設定する
    window.setViewport();
    
    // カラーバッファとデプスバッファを消去する
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    
    // 図形描画用のシェーダプログラムの使用を開始する
    point.use();
    
    // 図形表示用の投影変換行列を現在のウィンドウのアスペクトに合わせて設定する
    const auto mp(ggPerspective(1.0f, window.getAspect(), 1.0f, 5.0f));

    // 変換行列を設定する
    point.loadMatrix(mp, mv * window.getLeftTrackball());
    
    // 図形を描画する
    glBindVertexArray(vao);
    glDrawArrays(GL_POINTS, 0, points);
    
    // カラーバッファを入れ替えてイベントを取り出す
    window.swapBuffers();
  }
}
