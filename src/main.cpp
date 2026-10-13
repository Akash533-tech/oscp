#include "gui/App.h"
#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>
#include <GLFW/glfw3.h>
#include <cstdio>

static void glfwError(int code,const char*description){std::fprintf(stderr,"GLFW error %d: %s\n",code,description);}
int main(){glfwSetErrorCallback(glfwError);if(!glfwInit())return 1;
#if defined(__APPLE__)
 const char*glsl="#version 150";glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR,3);glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR,2);glfwWindowHint(GLFW_OPENGL_PROFILE,GLFW_OPENGL_CORE_PROFILE);glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT,GL_TRUE);
#else
 const char*glsl="#version 130";glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR,3);glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR,0);
#endif
 GLFWwindow*window=glfwCreateWindow(1440,900,"Mini Operating System Simulator",nullptr,nullptr);if(!window){glfwTerminate();return 2;}glfwMakeContextCurrent(window);glfwSwapInterval(1);IMGUI_CHECKVERSION();ImGui::CreateContext();ImGuiIO&io=ImGui::GetIO();io.ConfigFlags|=ImGuiConfigFlags_NavEnableKeyboard;io.FontGlobalScale=1.0f;ImGui_ImplGlfw_InitForOpenGL(window,true);ImGui_ImplOpenGL3_Init(glsl);minios::gui::App app;
 while(!glfwWindowShouldClose(window)){glfwPollEvents();ImGui_ImplOpenGL3_NewFrame();ImGui_ImplGlfw_NewFrame();ImGui::NewFrame();app.render(window);ImGui::Render();int w,h;glfwGetFramebufferSize(window,&w,&h);glViewport(0,0,w,h);glClearColor(.07f,.08f,.10f,1);glClear(GL_COLOR_BUFFER_BIT);ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());glfwSwapBuffers(window);}ImGui_ImplOpenGL3_Shutdown();ImGui_ImplGlfw_Shutdown();ImGui::DestroyContext();glfwDestroyWindow(window);glfwTerminate();return 0;}
