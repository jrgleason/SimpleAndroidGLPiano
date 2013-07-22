/*
 * Copyright (C) 2009 The Android Open Source Project
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

// OpenGL ES 2.0 code
#include <jni.h>
#include <android/log.h>

#include <GLES2/gl2.h>
#include <GLES2/gl2ext.h>

#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#define  LOG_TAG    "libgl2jni"
#define  LOGI(...)  __android_log_print(ANDROID_LOG_INFO,LOG_TAG,__VA_ARGS__)
#define  LOGE(...)  __android_log_print(ANDROID_LOG_ERROR,LOG_TAG,__VA_ARGS__)

static void printGLString(const char *name, GLenum s) {
	const char *v = (const char *) glGetString(s);
	LOGI("GL %s = %s\n", name, v);
}

static void checkGlError(const char* op) {
	for (GLint error = glGetError(); error; error = glGetError()) {
		LOGI("after %s() glError (0x%x)\n", op, error);
	}
}

static const char gVertexShader[] = "attribute vec4 vPosition;\n"
		"varying vec4 fragColor;                   \n"
		"attribute vec4 inColor;   \n"
		"void main() {\n"
		"  gl_Position = vPosition;\n"
		"  fragColor = inColor; \n"
		"}\n";

static const char gFragmentShader[] = "precision mediump float;\n"
		"varying vec4 fragColor;                   \n"
		"void main() {\n"
		"  gl_FragColor = fragColor;\n"
		"}\n";

GLuint loadShader(GLenum shaderType, const char* pSource) {
	GLuint shader = glCreateShader(shaderType);
	if (shader) {
		glShaderSource(shader, 1, &pSource, NULL);
		glCompileShader(shader);
		GLint compiled = 0;
		glGetShaderiv(shader, GL_COMPILE_STATUS, &compiled);
		if (!compiled) {
			GLint infoLen = 0;
			glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &infoLen);
			if (infoLen) {
				char* buf = (char*) malloc(infoLen);
				if (buf) {
					glGetShaderInfoLog(shader, infoLen, NULL, buf);
					LOGE("Could not compile shader %d:\n%s\n", shaderType, buf);
					free(buf);
				}
				glDeleteShader(shader);
				shader = 0;
			}
		}
	}
	return shader;
}

GLuint createProgram(const char* pVertexSource, const char* pFragmentSource) {
	GLuint vertexShader = loadShader(GL_VERTEX_SHADER, pVertexSource);
	if (!vertexShader) {
		return 0;
	}

	GLuint pixelShader = loadShader(GL_FRAGMENT_SHADER, pFragmentSource);
	if (!pixelShader) {
		return 0;
	}

	GLuint program = glCreateProgram();
	if (program) {
		glAttachShader(program, vertexShader);
		checkGlError("glAttachShader");
		glAttachShader(program, pixelShader);
		checkGlError("glAttachShader");
		glLinkProgram(program);
		GLint linkStatus = GL_FALSE;
		glGetProgramiv(program, GL_LINK_STATUS, &linkStatus);
		if (linkStatus != GL_TRUE) {
			GLint bufLength = 0;
			glGetProgramiv(program, GL_INFO_LOG_LENGTH, &bufLength);
			if (bufLength) {
				char* buf = (char*) malloc(bufLength);
				if (buf) {
					glGetProgramInfoLog(program, bufLength, NULL, buf);
					LOGE("Could not link program:\n%s\n", buf);
					free(buf);
				}
			}
			glDeleteProgram(program);
			program = 0;
		}
	}
	return program;
}

GLuint gProgram;
GLuint gvPositionHandle;
GLuint gvColorHandle;

bool setupGraphics(int w, int h) {
	printGLString("Version", GL_VERSION);
	printGLString("Vendor", GL_VENDOR);
	printGLString("Renderer", GL_RENDERER);
	printGLString("Extensions", GL_EXTENSIONS);

	//LOGI("setupGraphics(%d, %d)", w, h);
	gProgram = createProgram(gVertexShader, gFragmentShader);
	if (!gProgram) {
		LOGE("Could not create program.");
		return false;
	}
	gvPositionHandle = glGetAttribLocation(gProgram, "vPosition");
	gvColorHandle = glGetAttribLocation(gProgram, "inColor");
	checkGlError("glGetAttribLocation");
	//LOGI("glGetAttribLocation(\"vPosition\") = %d\n", gvPositionHandle);
	//LOGI("glGetAttribLocation(\"inColor\") = %d\n", gvColorHandle);

	glViewport(0, 0, w, h);
	checkGlError("glViewport");
	return true;
}

//const GLfloat gTriangleVertices1[] = { 0.0f, 0.0f,0.0f, 0.5f,  0.5f, 0.0f, 0.5, 0.5f};
const GLfloat gTriangleVertices1[] = { 0.2f, 0.6f, 0.2f, 0.9f, 0.7f, 0.6f, 0.7,
		0.9f };
const GLfloat gTriangleVertices2[] = { -0.2f, -0.1f, -0.2f, -0.4f, -0.7f, -0.1f,
		-0.7, -0.4f };
const GLfloat gTriangleVertices3[] = { -0.2f, 0.6f, -0.2f, 0.9f, -0.7f, 0.6f,
		-0.7, 0.9f };

const GLfloat gTriangleVertices4[] = { 0.2f, 0.4f, 0.2f, 0.1f, 0.7f, 0.4f, 0.7,
		0.1f };

const GLfloat gTriangleVertices5[] = { 0.2f, -0.1f, 0.2f, -0.4f, 0.7f, -0.1f,
		0.7, -0.4f };

const GLfloat gTriangleVertices6[] = { -0.2f, 0.4f, -0.2f, 0.1f, -0.7f, 0.4f,
		-0.7, 0.1f };

const GLfloat gTriangleVertices7[] = { -0.25f, -0.85f, -0.25f, -0.55f, 0.25f,
		-0.85f, 0.25f, -0.55f };

//const GLfloat gTriangleVertices5[] = { 0.0f, 0.0f, 0.0f, 0.2f, 0.5f, 0.0f, 0.5,
//		0.2f };
//const GLfloat gTriangleVertices2[] = { -0.2f, -0.2f,-0.2f, 0.3f,  0.3f, -0.2f, 0.3, 0.3f};

GLfloat red[] = { 1.0f, 0.0f, 0.0f, 1.0f, 1.0f, 0.0f, 0.0f, 1.0f, 1.0f, 0.0f,
		0.0f, 1.0f, 1.0f, 0.0f, 0.0f, 1.0f };

GLfloat green[] = { 0.0f, 1.0f, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f, 1.0f,
		0.0f, 1.0f, 0.0f, 1.0f, 0.0f, 1.0f };

GLfloat blue[] = { 0.0f, 0.0f, 1.0f, 1.0f, 0.0f, 0.0f, 1.0f, 1.0f, 0.0f, 0.0f,
		1.0f, 1.0f, 0.0f, 0.0f, 1.0f, 1.0f };

GLfloat yellow[] = { 1.0f, 1.0f, 0.0f, 1.0f, 1.0f, 1.0f, 0.0f, 1.0f, 1.0f, 1.0f,
		0.0f, 1.0f, 1.0f, 1.0f, 0.0f, 1.0f };

//GLfloat* current = &yellow[0];
GLfloat* current1 = &yellow[0];
GLfloat* current2 = &yellow[0];
GLfloat* current3 = &yellow[0];
GLfloat* current4 = &yellow[0];
GLfloat* current5 = &yellow[0];
GLfloat* current6 = &yellow[0];
GLfloat* current7 = &yellow[0];

void renderFrame() {
	static float grey;
//    grey += 0.01f;
//    if (grey > 1.0f) {
//        grey = 0.0f;
//    }
	glClearColor(grey, grey, grey, 1.0f);
	checkGlError("glClearColor");
	glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);
	checkGlError("glClear");

	glUseProgram(gProgram);
	checkGlError("glUseProgram");

	//LOGI("Color Handle is (\"inColor\") = %d\n", gvColorHandle);

	glVertexAttribPointer(gvPositionHandle, 2, GL_FLOAT, GL_FALSE, 0,
			gTriangleVertices1);
	glVertexAttribPointer(gvColorHandle, 4, GL_FLOAT, GL_FALSE, 0, current1);
	checkGlError("glVertexAttribPointer");
	glEnableVertexAttribArray(gvPositionHandle);
	glEnableVertexAttribArray(gvColorHandle);
	checkGlError("glEnableVertexAttribArray");
	glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);

	glVertexAttribPointer(gvColorHandle, 4, GL_FLOAT, GL_FALSE, 0, current3);
	glVertexAttribPointer(gvPositionHandle, 2, GL_FLOAT, GL_FALSE, 0,
			gTriangleVertices3);
	checkGlError("glVertexAttribPointer");
	glEnableVertexAttribArray(gvPositionHandle);
	checkGlError("glEnableVertexAttribArray");
	glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
	checkGlError("glDrawArrays");

//	glVertexAttribPointer(gvColorHandle, 4, GL_FLOAT, GL_FALSE, 0, blue);
	glVertexAttribPointer(gvColorHandle, 4, GL_FLOAT, GL_FALSE, 0, current4);
	glVertexAttribPointer(gvPositionHandle, 2, GL_FLOAT, GL_FALSE, 0,
			gTriangleVertices4);
	checkGlError("glVertexAttribPointer");
	glEnableVertexAttribArray(gvPositionHandle);
	checkGlError("glEnableVertexAttribArray");
	glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
	checkGlError("glDrawArrays");

	glVertexAttribPointer(gvColorHandle, 4, GL_FLOAT, GL_FALSE, 0, current6);
	glVertexAttribPointer(gvPositionHandle, 2, GL_FLOAT, GL_FALSE, 0,
			gTriangleVertices6);
	checkGlError("glVertexAttribPointer");
	glEnableVertexAttribArray(gvPositionHandle);
	checkGlError("glEnableVertexAttribArray");
	glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
	checkGlError("glDrawArrays");

//	glVertexAttribPointer(gvColorHandle, 4, GL_FLOAT, GL_FALSE, 0, green);

	glVertexAttribPointer(gvColorHandle, 4, GL_FLOAT, GL_FALSE, 0, current2);
	glVertexAttribPointer(gvPositionHandle, 2, GL_FLOAT, GL_FALSE, 0,
			gTriangleVertices2);
	checkGlError("glVertexAttribPointer");
	glEnableVertexAttribArray(gvPositionHandle);
	checkGlError("glEnableVertexAttribArray");
	glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
	checkGlError("glDrawArrays");

	glVertexAttribPointer(gvColorHandle, 4, GL_FLOAT, GL_FALSE, 0, current5);
	glVertexAttribPointer(gvPositionHandle, 2, GL_FLOAT, GL_FALSE, 0,
			gTriangleVertices5);
	checkGlError("glVertexAttribPointer");
	glEnableVertexAttribArray(gvPositionHandle);
	checkGlError("glEnableVertexAttribArray");
	glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
	checkGlError("glDrawArrays");

//	glVertexAttribPointer(gvColorHandle, 4, GL_FLOAT, GL_FALSE, 0, yellow);
	glVertexAttribPointer(gvColorHandle, 4, GL_FLOAT, GL_FALSE, 0, current7);
	glVertexAttribPointer(gvPositionHandle, 2, GL_FLOAT, GL_FALSE, 0,
			gTriangleVertices7);
	checkGlError("glVertexAttribPointer");
	glEnableVertexAttribArray(gvPositionHandle);
	checkGlError("glEnableVertexAttribArray");
	glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
	checkGlError("glDrawArrays");
}

int checkClick(float x, float y, float const* vertex, int count) {
	int i;
	int i2 = 0;
	float maxX = 0;
	int maxXSet = 0;
	float minX = 0;
	int minXSet = 0;
	float maxY = 0;
	int maxYSet = 0;
	float minY = 0;
	int minYSet = 0;
	for (i = 0; i < count; i++) {
		float x = vertex[i++];
		float y = vertex[i];
		if (x > maxX || maxXSet == 0) {
			maxX = x;
			maxXSet = 1;
		} else if (x < minX || minYSet == 0) {
			minX = x;
			minXSet = 1;
		}
		if (y > maxY || maxYSet == 0) {
			maxYSet = 1;
			maxY = y;
		} else if (y < minY || minYSet == 0) {
			minY = y;
			minYSet = 1;
		}
	}
	LOGI("x %f, y %f, maxX %f, maxY %f, minX %f, minY %f", x, y, maxX, maxY,
			minX, minY);
	if (x >= minX && x <= maxX && y >= minY && y <= maxY) {
		return 1;
	} else {
		return 0;
	}
}

extern "C" {
JNIEXPORT void JNICALL Java_com_android_gl2jni_GL2JNILib_init(JNIEnv * env, jobject obj, jint width, jint height);
JNIEXPORT void JNICALL Java_com_android_gl2jni_GL2JNILib_step(JNIEnv * env, jobject obj);
JNIEXPORT void JNICALL Java_com_android_gl2jni_GL2JNILib_touch(JNIEnv * env, jobject obj, jfloat x, jfloat y);
JNIEXPORT void JNICALL Java_com_android_gl2jni_GL2JNILib_touchUp(JNIEnv * env, jobject obj, jfloat x, jfloat y);
}
;

JNIEXPORT void JNICALL Java_com_android_gl2jni_GL2JNILib_init(JNIEnv * env, jobject obj, jint width, jint height)
{
	setupGraphics(width, height);
}

JNIEXPORT void JNICALL Java_com_android_gl2jni_GL2JNILib_touch(JNIEnv * env, jobject obj, jfloat x, jfloat y)
{
  LOGI("Touch Recieved %f,%f", x, y);


  if(checkClick(x,y,&gTriangleVertices1[0],8)==1){
	  LOGI("Clicked 1");
    current1 = &red[0];
    jclass clazz = env->FindClass("com/android/gl2jni/GL2JNILib");
    	  	  	      	  //jclass clazz = env->GetObjectClass(obj);
    	  	  	      jmethodID method = env->GetMethodID(clazz, "playB", "()V");
    	  	  	      env->CallVoidMethod(obj, method);
    //jmethodID method = env->GetMethodID(objclass, "PlayG", "(m_SizeClass)V");
  }
  else if(checkClick(x,y, &gTriangleVertices2[0],8)==1){
	  jclass clazz = env->FindClass("com/android/gl2jni/GL2JNILib");
	  	  	  	      	  //jclass clazz = env->GetObjectClass(obj);
	  	  	  	      jmethodID method = env->GetMethodID(clazz, "playE", "()V");
	  	  	  	      env->CallVoidMethod(obj, method);
	  LOGI("Clicked 2");
	  current2 = &red[0];
  }
  else if(checkClick(x,y,&gTriangleVertices3[0],8)==1){
	  LOGI(
	"Clicked 3");
	  jclass clazz = env->FindClass("com/android/gl2jni/GL2JNILib");
	      	  //jclass clazz = env->GetObjectClass(obj);
	      jmethodID method = env->GetMethodID(clazz, "playA", "()V");
	      env->CallVoidMethod(obj, method);

	  current3 =  &red[0];
  }
  else if(checkClick(x,y,&gTriangleVertices4[0],8)==1){
	  LOGI("Clicked 4");
	  jclass clazz = env->FindClass("com/android/gl2jni/GL2JNILib");
	  	  	  	  	      	  //jclass clazz = env->GetObjectClass(obj);
	  	  	  	  	      jmethodID method = env->GetMethodID(clazz, "playD", "()V");
	  	  	  	  	      env->CallVoidMethod(obj, method);
	  	  LOGI("Clicked 2");
	  current4 =  &red[0];
  }
  else if(checkClick(x,y,&gTriangleVertices5[0],8)==1){
	  LOGI("Clicked 5");
	  jclass clazz = env->FindClass("com/android/gl2jni/GL2JNILib");
	  	  	  	  	      	  //jclass clazz = env->GetObjectClass(obj);
	  	  	  	  	      jmethodID method = env->GetMethodID(clazz, "playFSharp", "()V");
	  	  	  	  	      env->CallVoidMethod(obj, method);
	  	  LOGI("Clicked 2");
	  current5 =  &red[0];
  }
  else if(checkClick(x,y,&gTriangleVertices6[0],8)==1){
	  LOGI("Clicked 6");
	  jclass clazz = env->FindClass("com/android/gl2jni/GL2JNILib");
	  	  	  	      	  //jclass clazz = env->GetObjectClass(obj);
	  	  	  	      jmethodID method = env->GetMethodID(clazz, "playC", "()V");
	  	  	  	      env->CallVoidMethod(obj, method);
	  current6 =  &red[0];
  }
  else if(checkClick(x,y,&gTriangleVertices7[0],8)==1){
	  LOGI(
	"Clicked 7")
;
	  jclass clazz = env->FindClass("com/android/gl2jni/GL2JNILib");
	  	      	  //jclass clazz = env->GetObjectClass(obj);
	  	      jmethodID method = env->GetMethodID(clazz, "playG", "()V");
	  	      env->CallVoidMethod(obj, method);
	  current7 =  &red[0];
  }
  //current = &red[0];
}

JNIEXPORT void JNICALL Java_com_android_gl2jni_GL2JNILib_touchUp(JNIEnv * env, jobject obj, jfloat x, jfloat y)
{
  LOGI("Touch Recieved %f,%f", x, y)
;

current1 = &yellow[0];
current2 = &yellow[0];
current3 = &yellow[0];
current4 = &yellow[0];
current5 = &yellow[0];
current6 = &yellow[0];
current7 = &yellow[0];
}

JNIEXPORT void JNICALL Java_com_android_gl2jni_GL2JNILib_step(JNIEnv * env, jobject obj)
{
renderFrame();
}
