#version 150 core
#extension GL_ARB_explicit_attrib_location : enable

//
// point.vert
//
//   �_��`���V�F�[�_
//

// �ϊ��s��
uniform mat4 mc;                                      // �N���b�s���O���W�n�ւ̕ϊ��s��

// ���_����
layout (location = 0) in vec4 pv;                     // ���[�J�����W�n�̒��_�ʒu

void main(void)
{
  gl_Position = mc * vec4(pv.xyz * pv.w, 1.0);
}
