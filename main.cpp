// ImGui - standalone example application for SDL2 + OpenGL
// If you are new to ImGui, see examples/README.txt and documentation at the top of imgui.cpp.

#include "imgui/imgui.h"
#include "imgui_impl_sdl_gl3.h"
#include "gl3w/GL/gl3w.h"
#include "SDL2/SDL.h"
#include "SDL2/SDL_opengl.h"

#include <stdio.h>
#include <windows.h>
#include <shellapi.h>
#include <tchar.h>
#include <direct.h>

#define MAXSTR 4096

bool G_AppRunning = true;
int G_WindowWidth = 640;
int G_WindowHeight = 480;

char *CreateAppArgsOutput(const char *input)
{
  int len = strlen(input);
  if(!input || len < 1) return NULL;
  int slashes = 0;
  int quotes = 0;
  for(int i = 0; i < len; ++i)
  {
    if(input[i] == '\\') ++slashes;
    if(input[i] == '\"') ++quotes;
  }
  
  // Magic 6 for \"wrapping quotes\" and \0
  char *output = (char *)calloc(len + (slashes * 2) + (quotes * 6) + 6, sizeof(char));
  if(!output) return NULL;

  int j = 0;
  for(int i = 0; i < len; ++i, ++j)
  {
    if(input[i] == '\"')
    {
      output[j] = '\\';
      ++j;
    }
    output[j] = input[i];
    if(input[i] == '\\')
    {
      ++j;
      output[j] = '\\';
    }
  }

  printf("APP ARGS:\n\t%s\n\t%s\n", input, output);

  return output;
}

char *CreateAppPathOutput(const char *input)
{
  int len = strlen(input);
  if(!input || len < 1) return NULL;
  int slashes = 0;
  for(int i = 0; i < len; ++i)
  {
    if(input[i] == '\\') ++slashes;
  }
  
  // Magic 6 for \"wrapping quotes\" and \0
  char *output = (char *)calloc(len + (slashes * 2) + 6, sizeof(char));
  if(!output) return NULL;
  output[0] ='\\';
  output[1] ='\"';

  int j = 2;
  for(int i = 0; i < len; ++i, ++j)
  {
    output[j] = input[i];
    if(input[i] == '\\') 
    {
      ++j;
      output[j] = '\\';
    }
  }
  output[j++] = '\\';
  output[j] = '\"';

  printf("APP PATH:\n\t%s\n\t%s\n", input, output);

  return output;
}

void Generate(const char *app_path, const char *app_args, const char *exe_name, bool admin)
{
  printf("Generating file: ~gen.c %s\n%s %s\n", exe_name, app_path, app_args);
  char *app_path_output = CreateAppPathOutput(app_path);
  char *app_args_output = CreateAppArgsOutput(app_args);
  _mkdir("gen");
  FILE *gen_file = fopen("gen\\~gen.c", "w");
  if(gen_file)
  {
    fprintf(gen_file, "#include <stdio.h>\n#include <windows.h>\n#include <tchar.h>\n");
    fprintf(gen_file, "int CALLBACK WinMain(HINSTANCE h, HINSTANCE p, LPSTR cmd, int show) {\n");
    fprintf(gen_file, "\tSHELLEXECUTEINFO shex = {0};\n");
    fprintf(gen_file, "\tshex.cbSize = sizeof(shex);\n");
    fprintf(gen_file, "\tshex.fMask = SEE_MASK_NOCLOSEPROCESS;\n");
    fprintf(gen_file, "\tshex.hwnd = 0;\n");
    if(admin) fprintf(gen_file, "  shex.lpVerb = _T(\"runas\");\n");
    fprintf(gen_file, "\tshex.lpFile = _T(\"%s\");\n", app_path_output);
    fprintf(gen_file, "\tshex.lpParameters = _T(\"%s\");\n", app_args_output);
    fprintf(gen_file, "\tshex.lpDirectory = 0;\n");
    fprintf(gen_file, "\tshex.nShow = SW_HIDE;\n");
    fprintf(gen_file, "\tshex.hInstApp = 0;\n");
    fprintf(gen_file, "\tif(ShellExecuteEx(&shex)) {\n");
    fprintf(gen_file, "\t\tWaitForSingleObject(shex.hProcess, INFINITE);\n");
    fprintf(gen_file, "\t\tCloseHandle(shex.hProcess);\n");
    fprintf(gen_file, "\t}\n");
    fprintf(gen_file, "\treturn 0;\n");
    fprintf(gen_file, "}\n");
    fclose(gen_file);


    char *cl_cmd = "cl";
    char cl_arg[4096] = {};
    strcat(cl_arg, " gen\\~gen.c /Fegen\\");
    strcat(cl_arg, exe_name);
    strcat(cl_arg, " /link shell32.lib user32.lib /SUBSYSTEM:WINDOWS");
    char command[4096] = {};
    strcat(command, cl_cmd);
    strcat(command, cl_arg);

    system("\"\"C:\\Program Files (x86)\\Microsoft Visual Studio\\2017\\Community\\VC\\Auxiliary\\Build\\vcvarsall.bat\" x64\"");
    system(command);

    printf("Finished.\n");
  }
  else
  {
    printf("Error opening code gen file.\n");
  }
}

static void ShowHelpMarker(const char *desc)
{
  ImGui::SameLine();
  ImGui::TextDisabled("(?)");
  if(ImGui::IsItemHovered())
  {
    ImGui::BeginTooltip();
    ImGui::PushTextWrapPos(200.0f);
    ImGui::TextUnformatted(desc);
    ImGui::PopTextWrapPos();
    ImGui::EndTooltip();
  }
}

void ImGui_MainUI()
{
  ImGui::SetNextWindowPos(ImVec2(0,0));
  ImGui::SetNextWindowSize(ImVec2(G_WindowWidth, G_WindowHeight));
  ImGui::Begin("Main Window", NULL, ImGuiWindowFlags_NoResize|ImGuiWindowFlags_NoMove|
               ImGuiWindowFlags_NoTitleBar);
  {
    static char app_path[MAXSTR] = "C:\\SCRIPTS\\nircmdc.exe";
    ImGui::InputText("App Path", app_path, MAXSTR);
    ShowHelpMarker("The path of the application you want to run.");

    static char app_args[MAXSTR] = "setdefaultsounddevice \"Audio Device\"";
    ImGui::InputText("App Args", app_args, MAXSTR);
    ShowHelpMarker("The arguments you want to pass to the application (optional).");

    static char exe_name[MAXSTR] = "AudioDevice.exe";
    ImGui::InputText("Ouput Name", exe_name, MAXSTR);
    ShowHelpMarker("The output name of the executable you want to create.");

    static bool admin = false;
    ImGui::Checkbox("Requires admin privledges?", &admin);
    ShowHelpMarker("If the app you want to run requires admin privledges, check this box.");

    if(ImGui::Button("Generate"))
    {
      if(!strcmp("", exe_name)) ImGui::OpenPopup("Ouput Name Error");
      else if(!strcmp("", app_path)) ImGui::OpenPopup("App Path Error");
      Generate(app_path, app_args, exe_name, admin);
    }

    if(ImGui::BeginPopupModal("Ouput Name Error", NULL, ImGuiWindowFlags_AlwaysAutoResize))
    {
      ImGui::SetNextWindowPosCenter();
      ImGui::Text("You must specify an executable name.");
      ImVec2 m = ImGui::GetItemRectSize();
      if(ImGui::Button("OK", ImVec2(m.x, 0))) ImGui::CloseCurrentPopup();
      ImGui::EndPopup();
    }
    else if(ImGui::BeginPopupModal("App Path Error"))
    {
      ImGui::SetNextWindowPosCenter();
      ImGui::Text("You must specify an application path.");
      ImVec2 m = ImGui::GetItemRectSize();
      if(ImGui::Button("OK", ImVec2(m.x, 0))) ImGui::CloseCurrentPopup();
      ImGui::EndPopup();
    }
  }
  ImGui::End();
}

void ImGui_Render()
{
  glViewport(0, 0, (int)ImGui::GetIO().DisplaySize.x, (int)ImGui::GetIO().DisplaySize.y);
  glClearColor(0.0f, 0.0f, 0.0f, 1.00f);
  glClear(GL_COLOR_BUFFER_BIT);
  ImGui::Render();
}

void HandleEvents(SDL_Window *window)
{
  SDL_GetWindowSize(window, &G_WindowWidth, &G_WindowHeight);
  SDL_Event event;
  while(SDL_PollEvent(&event))
  {
    ImGui_ImplSdlGL3_ProcessEvent(&event);
    switch(event.type)
    {
      case SDL_QUIT: G_AppRunning = false; break;
      case SDL_DROPFILE:
      {

      } break;
      case SDL_KEYDOWN:
      {
        switch(event.key.keysym.sym)
        {
        }
      } break;
    }
  }
  ImGui_ImplSdlGL3_NewFrame(window);
}

int main(int argc, char **argv)
{
  
  // Setup SDL
  if (SDL_Init(SDL_INIT_VIDEO) != 0)
  {
    printf("Error: %s\n", SDL_GetError());
    return -1;
  }

  // Setup window
  SDL_GL_SetAttribute(SDL_GL_CONTEXT_FLAGS, SDL_GL_CONTEXT_FORWARD_COMPATIBLE_FLAG);
  SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
  SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
  SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);
  SDL_GL_SetAttribute(SDL_GL_STENCIL_SIZE, 8);
  SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
  SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 2);
  SDL_DisplayMode current;
  SDL_GetCurrentDisplayMode(0, &current);
  SDL_Window *window = SDL_CreateWindow("Silent Command Generator",
                                        SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 
                                        G_WindowWidth, G_WindowHeight,
                                        SDL_WINDOW_RESIZABLE|
                                        SDL_WINDOW_OPENGL);
  SDL_GLContext glcontext = SDL_GL_CreateContext(window);
  SDL_GetWindowSize(window, &G_WindowWidth, &G_WindowHeight);
  gl3wInit();
  ImGui_ImplSdlGL3_Init(window);

  ImGuiStyle& style = ImGui::GetStyle();
  ImGui::StyleColorsDark(&style);

  bool p_open;
  ImVec4 clear_color = ImColor(80, 80, 80);

  // Main loop
  while(G_AppRunning)
  {
    HandleEvents(window);

    ImGui_MainUI();

    ImGui_Render();
    SDL_GL_SwapWindow(window);
  }

  // Cleanup
  ImGui_ImplSdlGL3_Shutdown();
  SDL_GL_DeleteContext(glcontext);
  SDL_DestroyWindow(window);
  SDL_Quit();

  return 0;
}
