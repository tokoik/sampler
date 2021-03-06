#version 150 core
#extension GL_ARB_explicit_attrib_location : enable

//
// point.vert
//
//   点を描くシェーダ
//

// 変換行列
uniform mat4 mc;                                      // クリッピング座標系への変換行列

// 頂点属性
layout (location = 0) in vec4 pv;                     // ローカル座標系の頂点位置

void main(void)
{
  gl_Position = mc * vec4(pv.xyz * pv.w, 1.0);
}
