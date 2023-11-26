#include "cube.h"
#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtx/transform.hpp"
#include "texture.hpp"

glm::mat4 projectMat;
glm::mat4 viewMat;
glm::mat4 modelMat = glm::mat4(1.0f); 

GLuint pvmMatrixID;
GLuint projectMatrixID; 
GLuint viewMatrixID;
GLuint modelMatrixID;
GLuint textureModeID;
GLuint Texture;
GLuint TextureHead;

float rotAngle = 0.0f;
int isRotate = false;
int isTexture = false;

typedef glm::vec4  point4;
typedef glm::vec3  normal3;
typedef glm::vec2  texture2;

const int NumVertices = 36; // 큐브 6면 * 한 면 당 2개의 삼각형 * 삼각형 당 3개의 정점

point4 points[NumVertices];
normal3 normals[NumVertices]; // 법선 벡터 저장
texture2 texCoords[NumVertices]; // 텍스쳐 좌표 저장

// 원점을 중심으로 하는 큐브의 정점
point4 vertices[8] = {
	point4(-0.5, -0.5, 0.5, 1.0),
	point4(-0.5, 0.5, 0.5, 1.0),
	point4(0.5, 0.5, 0.5, 1.0),
	point4(0.5, -0.5, 0.5, 1.0),
	point4(-0.5, -0.5, -0.5, 1.0),
	point4(-0.5, 0.5, -0.5, 1.0),
	point4(0.5, 0.5, -0.5, 1.0),
	point4(0.5, -0.5, -0.5, 1.0)
};

//----------------------------------------------------------------------------

// quad()는 각 면에 대해 두 개의 삼각형을 생성하고 정점에 색상을 부여한다.
int Index = 0;
void quad(int a, int b, int c, int d)
{
	// 큐브 면의 법선 벡터 계산
	normal3 u = vertices[b] - vertices[a];
	normal3 v = vertices[c] - vertices[b];
	normal3 normal = glm::normalize(glm::cross(u, v));

	// 첫 번째 삼각형
	points[Index] = vertices[a]; normals[Index] = normal; texCoords[Index] = glm::vec2(0.0f, 1.0f); Index++;
	points[Index] = vertices[b]; normals[Index] = normal; texCoords[Index] = glm::vec2(1.0f, 1.0f); Index++;
	points[Index] = vertices[c]; normals[Index] = normal; texCoords[Index] = glm::vec2(1.0f, 0.0f); Index++;

	// 두 번째 삼각형
	points[Index] = vertices[a]; normals[Index] = normal; texCoords[Index] = glm::vec2(0.0f, 1.0f); Index++;
	points[Index] = vertices[c]; normals[Index] = normal; texCoords[Index] = glm::vec2(1.0f, 0.0f); Index++;
	points[Index] = vertices[d]; normals[Index] = normal; texCoords[Index] = glm::vec2(0.0f, 0.0f); Index++;
}

//----------------------------------------------------------------------------

// 12개의 삼각형을 생성한다. 총 정점은 36개
void colorcube()
{
	quad(1, 0, 3, 2);
	quad(2, 3, 7, 6);
	quad(3, 0, 4, 7);
	quad(6, 5, 1, 2);
	quad(4, 5, 6, 7);
	quad(5, 4, 0, 1);
}

//----------------------------------------------------------------------------

// OpenGL 초기화
void init()
{
	colorcube();

	// VAO 생성
	GLuint vao;
	glGenVertexArrays(1, &vao);
	glBindVertexArray(vao);

	// 버퍼 객체 생성 및 초기화
	GLuint buffer;
	glGenBuffers(1, &buffer);
	glBindBuffer(GL_ARRAY_BUFFER, buffer);

	// 버퍼 데이터를 정점 데이터와 법선 벡터 데이터로 설정한다.
	glBufferData(GL_ARRAY_BUFFER, sizeof(points) + sizeof(normals) + sizeof(texCoords), NULL, GL_STATIC_DRAW);
	glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(points), points);
	glBufferSubData(GL_ARRAY_BUFFER, sizeof(points), sizeof(normals), normals);
	glBufferSubData(GL_ARRAY_BUFFER, sizeof(points) + sizeof(normals), sizeof(texCoords), texCoords);

	// 셰이더를 로드하고 쉐이더 프로그램 사용
	GLuint program = InitShader("src/vshader.glsl", "src/fshader.glsl");
	glUseProgram(program);

	// 정점 배열 설정
	GLuint vPosition = glGetAttribLocation(program, "vPosition");
	glEnableVertexAttribArray(vPosition);
	glVertexAttribPointer(vPosition, 4, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(0));

	// 법선 벡터 설정
	GLuint vNormal = glGetAttribLocation(program, "vNormal");
	glEnableVertexAttribArray(vNormal);
	glVertexAttribPointer(vNormal, 3, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(sizeof(points)));

	// 텍스쳐 좌표 설정
	GLuint vTexCoord = glGetAttribLocation(program, "vTexCoord");
	glEnableVertexAttribArray(vTexCoord);
	glVertexAttribPointer(vTexCoord, 2, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(sizeof(points) + sizeof(normals)));

	projectMatrixID = glGetUniformLocation(program, "mProject");
	projectMat = glm::perspective(glm::radians(65.0f), 1.0f, 0.1f, 100.0f);
	glUniformMatrix4fv(projectMatrixID, 1, GL_FALSE, &projectMat[0][0]);

	viewMatrixID = glGetUniformLocation(program, "mView");
	viewMat = glm::lookAt(glm::vec3(0, 0, 2), glm::vec3(0, 0, 0), glm::vec3(0, 1, 0));
	glUniformMatrix4fv(viewMatrixID, 1, GL_FALSE, &viewMat[0][0]);

	modelMatrixID = glGetUniformLocation(program, "mModel");
	modelMat = glm::mat4(1.0f);
	glUniformMatrix4fv(modelMatrixID, 1, GL_FALSE, &modelMat[0][0]);

	pvmMatrixID = glGetUniformLocation(program, "mPVM");
	glm::mat4 pvmMat = projectMat * viewMat * modelMat;
	glUniformMatrix4fv(pvmMatrixID, 1, GL_FALSE, &pvmMat[0][0]);

	textureModeID = glGetUniformLocation(program, "isTexture");
	glUniform1i(textureModeID, isTexture);

	// 텍스쳐를 불러온다.
	Texture = loadBMP_custom("cat.bmp");
	TextureHead = loadBMP_custom("box.bmp");

	// "myTextureSampler" 유니폼에 대한 핸들을 얻는다.
	GLuint TextureID = glGetUniformLocation(program, "cubeTexture");

	// 텍스처 유닛 0에 텍스처를 바인딩
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, Texture);

	// Texture Unit 0을 사용하도록 "myTextureSampler" 샘플러를 설정
	glUniform1i(TextureID, 0);

	glEnable(GL_DEPTH_TEST);
	glClearColor(0.0, 0.0, 0.0, 1.0);
}

//----------------------------------------------------------------------------
void drawAnimal(glm::mat4 catMat)
{
	float time = glutGet(GLUT_ELAPSED_TIME) / 1000.0f; // 현재 시간을 초 단위로 가져옴.
	float legRotation = sin(time * 5.0f) * 0.5f; // 다리의 회전각도를 계산
	float tailRotation = sin(time * 25.0f) * 0.3f; // 꼬리의 회전각도를 계산

	glm::mat4 modelMat, pvmMat;
	glm::vec3 LFPos[2];
	glm::vec3 LRPos[2];
	glm::vec3 RFPos[2];
	glm::vec3 RRPos[2];
	glm::vec3 earPos[2];

	LFPos[0] = glm::vec3(0.25, -0.25, -0.125);  // 좌측 전면 허벅지
	LFPos[1] = glm::vec3(0.25, -0.35, -0.125);  // 좌측 전면 종아리
	LRPos[0] = glm::vec3(-0.25, -0.25, -0.125);   // 좌측 후면 허벅지
	LRPos[1] = glm::vec3(-0.25, -0.35, -0.125);	// 좌측 후면 종아리

	RFPos[0] = glm::vec3(0.25, -0.25, 0.125); // 우측 전면 허벅지
	RFPos[1] = glm::vec3(0.25, -0.35, 0.125); // 우측 전면 종아리
	RRPos[0] = glm::vec3(-0.25, -0.25, 0.125);  // 우측 후면 허벅지
	RRPos[1] = glm::vec3(-0.25, -0.35, 0.125);  // 우측 후면 종아리

	earPos[0] = glm::vec3(0.5, 0.35, 0.1); // 왼쪽 귀
	earPos[1] = glm::vec3(0.5, 0.35, -0.1); // 오른쪽 귀

	// 몸 
	modelMat = glm::scale(catMat, glm::vec3(0.6, 0.4, 0.4));
	pvmMat = projectMat * viewMat * modelMat;
	glUniformMatrix4fv(pvmMatrixID, 1, GL_FALSE, &pvmMat[0][0]);
	glDrawArrays(GL_TRIANGLES, 0, NumVertices);

	// 머리에 박스 텍스쳐 바인딩
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, TextureHead);

	// 머리
	modelMat = glm::translate(catMat, glm::vec3(0.425, 0.2, 0));
	modelMat = glm::scale(modelMat, glm::vec3(0.25, 0.25, 0.25));
	pvmMat = projectMat * viewMat * modelMat;
	glUniformMatrix4fv(pvmMatrixID, 1, GL_FALSE, &pvmMat[0][0]);
	glDrawArrays(GL_TRIANGLES, 0, NumVertices);

	// 머리 외 다른 부분은 고양이 텍스처로 재설정
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, Texture);

	// 꼬리
	glm::vec3 tailBasePos = glm::vec3(-0.3, 0, 0);  // 몸통의 뒷부분에 위치
	modelMat = glm::translate(catMat, tailBasePos);  // 몸통의 뒷부분 위치로 이동
	modelMat = glm::rotate(modelMat, tailRotation, glm::vec3(0, 1, 0));  // 회전 적용
	modelMat = glm::translate(modelMat, -tailBasePos + glm::vec3(-0.4, 0, 0));  // 원래 꼬리의 위치로 되돌린다
	modelMat = glm::scale(modelMat, glm::vec3(0.2, 0.1, 0.1));
	pvmMat = projectMat * viewMat * modelMat;
	glUniformMatrix4fv(pvmMatrixID, 1, GL_FALSE, &pvmMat[0][0]);
	glDrawArrays(GL_TRIANGLES, 0, NumVertices);

	// 좌측 전면부 다리 (LF)
	for (int i = 0; i < 2; i++)
	{
		glm::vec3 legRotationAxis = glm::vec3(0.25, -0.2, 0); // 몸통의 아랫부분 위치
		modelMat = glm::translate(catMat, legRotationAxis);  // 다리의 상단 위치로 이동
		modelMat = glm::rotate(modelMat, legRotation, glm::vec3(0, 0, 0.5));  // 회전 적용
		modelMat = glm::translate(modelMat, -legRotationAxis + LFPos[i]);  // 원래의 위치로 되돌린다
		modelMat = glm::scale(modelMat, glm::vec3(0.1, 0.1, 0.1));
		pvmMat = projectMat * viewMat * modelMat;
		glUniformMatrix4fv(pvmMatrixID, 1, GL_FALSE, &pvmMat[0][0]);
		glDrawArrays(GL_TRIANGLES, 0, NumVertices);
	}

	// 좌측 후면부 다리 (LR)
	for (int i = 0; i < 2; i++)
	{
		glm::vec3 legRotationAxis = glm::vec3(-0.25, -0.2, 0);
		modelMat = glm::translate(catMat, legRotationAxis);
		modelMat = glm::rotate(modelMat, legRotation, glm::vec3(0, 0, 0.5));
		modelMat = glm::translate(modelMat, -legRotationAxis + LRPos[i]);
		modelMat = glm::scale(modelMat, glm::vec3(0.1, 0.1, 0.1));
		pvmMat = projectMat * viewMat * modelMat;
		glUniformMatrix4fv(pvmMatrixID, 1, GL_FALSE, &pvmMat[0][0]);
		glDrawArrays(GL_TRIANGLES, 0, NumVertices);
	}

	// 우측 전면부 다리 (RF)
	for (int i = 0; i < 2; i++)
	{
		glm::vec3 legRotationAxis = glm::vec3(0.25, -0.2, 0);
		modelMat = glm::translate(catMat, legRotationAxis);
		modelMat = glm::rotate(modelMat, -legRotation, glm::vec3(0, 0, 0.5));
		modelMat = glm::translate(modelMat, -legRotationAxis + RFPos[i]);
		modelMat = glm::scale(modelMat, glm::vec3(0.1, 0.1, 0.1));
		pvmMat = projectMat * viewMat * modelMat;
		glUniformMatrix4fv(pvmMatrixID, 1, GL_FALSE, &pvmMat[0][0]);
		glDrawArrays(GL_TRIANGLES, 0, NumVertices);
	}

	// 우측 후면부 다리 (RR)
	for (int i = 0; i < 2; i++)
	{
		glm::vec3 legRotationAxis = glm::vec3(-0.25, -0.2, 0);
		modelMat = glm::translate(catMat, legRotationAxis);
		modelMat = glm::rotate(modelMat, -legRotation, glm::vec3(0, 0, 0.5));
		modelMat = glm::translate(modelMat, -legRotationAxis + RRPos[i]);
		modelMat = glm::scale(modelMat, glm::vec3(0.1, 0.1, 0.1));
		pvmMat = projectMat * viewMat * modelMat;
		glUniformMatrix4fv(pvmMatrixID, 1, GL_FALSE, &pvmMat[0][0]);
		glDrawArrays(GL_TRIANGLES, 0, NumVertices);
	}

	// 머리의 박스를 귀에도 적용
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, TextureHead);

	// 귀
	for (int i = 0; i < 2; i++)
	{
		modelMat = glm::translate(catMat, earPos[i]);
		modelMat = glm::scale(modelMat, glm::vec3(0.05, 0.05, 0.05));
		pvmMat = projectMat * viewMat * modelMat;
		glUniformMatrix4fv(pvmMatrixID, 1, GL_FALSE, &pvmMat[0][0]);
		glDrawArrays(GL_TRIANGLES, 0, NumVertices);
	}

	// 그 외 다른 부분은 고양이 텍스처로 재설정
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, Texture);
}

void display(void)
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	drawAnimal(modelMat);
	glUniformMatrix4fv(modelMatrixID, 1, GL_FALSE, &modelMat[0][0]);

	glutSwapBuffers();
}

//----------------------------------------------------------------------------

void idle()
{
	static int prevTime = glutGet(GLUT_ELAPSED_TIME);
	int currTime = glutGet(GLUT_ELAPSED_TIME);

	if (abs(currTime - prevTime) >= 20)
	{
		float t = abs(currTime - prevTime);
		float speed = 720.0f / 10000.0f;
		modelMat = glm::rotate(modelMat, glm::radians(t * speed), glm::vec3(1.0f, 1.0f, 0.0f));
		prevTime = currTime;
		glutPostRedisplay();
	}
}

//----------------------------------------------------------------------------

void
keyboard(unsigned char key, int x, int y)
{
	switch (key) {
	case 't': case 'T':
		isTexture = !isTexture;
		glUniform1i(textureModeID, isTexture);
		glutPostRedisplay();
		break;
	case 'r': case 'R':
		isRotate = !isRotate;
		glutPostRedisplay();
		break;
	case 033:  // Escape key
	case 'q': case 'Q':
		exit(EXIT_SUCCESS);
		break;
	}
}

//----------------------------------------------------------------------------

void resize(int w, int h)
{
	float ratio = (float)w / (float)h;
	glViewport(0, 0, w, h);

	projectMat = glm::perspective(glm::radians(65.0f), ratio, 0.1f, 100.0f);
	glUniformMatrix4fv(projectMatrixID, 1, GL_FALSE, &projectMat[0][0]);
	glutPostRedisplay();
}

//----------------------------------------------------------------------------

int
main(int argc, char** argv)
{
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_RGBA | GLUT_DOUBLE | GLUT_DEPTH);
	glutInitWindowSize(512, 512);
	glutInitContextVersion(3, 2);
	glutInitContextProfile(GLUT_CORE_PROFILE);
	glutCreateWindow("Color Cat");

	glewInit();

	init();

	glutDisplayFunc(display);
	glutKeyboardFunc(keyboard);
	glutReshapeFunc(resize);
	glutIdleFunc(idle);

	glutMainLoop();
	return 0;
}