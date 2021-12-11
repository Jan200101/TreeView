#include <SDL.h>
#include <SDL_opengl.h>

#include <unistd.h>
#include <limits.h>
#include <thread>
#include <libgen.h>

#include "imgui.h"
#include "imgui_impl_sdl.h"
#include "imgui_impl_opengl2.h"

#include "main.h"
#include "common.h"
#include "config.h"
#include "tree.h"
#include "threads.h"

int done = 0;
SDL_Window* window;
ImGuiIO* io;

char root_dir[PATH_MAX] = {0};
bool showHiddenFiles = false;


static void spawnProcess(struct FileTree* file)
{
    pid_t pid = fork();

    if (!pid)
    {
        // child

        switch(file->type)
        {
            case FileTree::FILE:
                if (isExe(file->name))
                    execl(file->name, file->name, NULL);
                else
                    execl("/usr/bin/env", "env", "xdg-open", file->name, NULL);
                break;

            case FileTree::UNKNOWN:
                // don't know, try xdg-open
            case FileTree::DIRECTORY:
                // open the directory with the users prefered File Manager
                execl("/usr/bin/env", "env", "xdg-open", file->name, NULL);
                break;

            case FileTree::NONE:
            default:
                break;
        }
        // We can't deal with your errors so just take your bags and leave
        exit(0);
    }

}

static void createContextMenu(struct FileTree* tree)
{
    if (ImGui::BeginPopupContextItem()) // <-- use last item id as popup id
    {
        ImGui::Text("%s ISDIR(%i) ISFILE(%i)", tree->name, isDir(tree->name), isFile(tree->name));
        if (ImGui::Button("Open"))
        {
            puts(tree->name);

            spawnProcess(tree);
            ImGui::CloseCurrentPopup();
        }

        if (isFile(tree->name) && ImGui::Button("Delete"))
        {
            unlink(tree->name);
            tree->type = FileTree::NONE;
            ImGui::CloseCurrentPopup();
        }        
        ImGui::EndPopup();
    }
}

static void buildTree(struct FileTree* tree)
{
    char* name = basename(tree->name);
    if (!showHiddenFiles && name[0] == '.') return;

    switch(tree->type)
    {
        case FileTree::DIRECTORY:
            if (ImGui::TreeNode(name))
            {
                createContextMenu(tree);
                if (!tree->value)
                {
                    // the tree is not complete.
                    // Spin up a thread and come back later
                    spawnTreeThread(tree, tree->name, strlen(tree->name), 2);
                }
                else
                {
                    for (size_t i = 0; i < tree->count; ++i)
                    {
                        buildTree(&tree->value[i]);
                    }
                }
                ImGui::TreePop();
            }
            else createContextMenu(tree);
            break;

        case FileTree::FILE:
        case FileTree::UNKNOWN:
            ImGui::TreeNodeEx(name, ImGuiTreeNodeFlags_Leaf | ImGuiTreeNodeFlags_NoTreePushOnOpen);
            createContextMenu(tree);
            break;

        default:
            return;
    }
};

static void createTreeView()
{
    const ImGuiViewport* viewport = ImGui::GetMainViewport();
    ImGui::SetNextWindowPos(viewport->WorkPos);
    ImGui::SetNextWindowSize(viewport->WorkSize);
    ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
    ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);

    ImGuiWindowFlags window_flags = ImGuiWindowFlags_MenuBar | ImGuiWindowFlags_NoTitleBar;
    window_flags |=  ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove;
    window_flags |= ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoNavFocus;

    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));
    ImGui::Begin("#view", NULL, window_flags);

    ImGui::PopStyleVar();
    ImGui::PopStyleVar(2);

    ImGui::SetNextItemOpen(true, ImGuiCond_Once);

    buildTree(&global_tree);


    if (ImGui::BeginMenuBar())
    {
        if (ImGui::BeginMenu("Options"))
        {

            ImGui::MenuItem("Show Hidden Files", NULL, &showHiddenFiles);
            ImGui::EndMenu();
        }

        ImGui::EndMenuBar();
    }

    ImGui::End();

}

void processSDLEvent()
{
    SDL_Event event;
    while (SDL_PollEvent(&event))
    {
        ImGui_ImplSDL2_ProcessEvent(&event);
        if (event.type == SDL_QUIT)
            done = 1;
        else if (event.type == SDL_WINDOWEVENT)
        {
            if (event.window.event == SDL_WINDOWEVENT_CLOSE && event.window.windowID == SDL_GetWindowID(window))
                done = 1;
        }
    }
}

int init()
{
    // Setup window
    SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
    SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);
    SDL_GL_SetAttribute(SDL_GL_STENCIL_SIZE, 8);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 2);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 2);
    SDL_WindowFlags window_flags = (SDL_WindowFlags)(SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE | SDL_WINDOW_ALLOW_HIGHDPI);
    window = SDL_CreateWindow(NAME, SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, 640, 480, window_flags);

    if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_TIMER) != 0)
    {
        printf("Error: %s\n", SDL_GetError());
        return -1;
    }


    // setup tree to ensure we don't access random memory while we are creating the tree
    global_tree.name = NULL;
    global_tree.type = FileTree::UNKNOWN;
    global_tree.count = 0;
    global_tree.value = NULL;

    return 0;
}

int app(int, char**)
{


    SDL_GLContext gl_context = SDL_GL_CreateContext(window);
    SDL_GL_MakeCurrent(window, gl_context);
    SDL_GL_SetSwapInterval(1);

    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    io = &ImGui::GetIO();
    io->ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;

    ImGui::StyleColorsDark();
    //ImGuiStyle& style = ImGui::GetStyle();

    ImGui_ImplSDL2_InitForOpenGL(window, gl_context);
    ImGui_ImplOpenGL2_Init();

    while (!done)
    {
        processSDLEvent();

        ImGui_ImplOpenGL2_NewFrame();
        ImGui_ImplSDL2_NewFrame();
        ImGui::NewFrame();

        //ImGui::ShowDemoWindow(NULL);

        createTreeView();

        ImGui::Render();
        glViewport(0, 0, (int)io->DisplaySize.x, (int)io->DisplaySize.y);
        glClear(GL_COLOR_BUFFER_BIT);

        ImGui_ImplOpenGL2_RenderDrawData(ImGui::GetDrawData());

        SDL_GL_SwapWindow(window);
    }

    ImGui_ImplOpenGL2_Shutdown();
    ImGui_ImplSDL2_Shutdown();
    ImGui::DestroyContext();

    SDL_GL_DeleteContext(gl_context);
    SDL_DestroyWindow(window);
    SDL_Quit();

    return 0;
}

int main(int c, char** v)
{
    init();

    if(!getLastPath(root_dir, sizeof(root_dir))) getcwd(root_dir, sizeof(root_dir));


    if (c > 1)
    {
        if (v[1][0] != PATH_SEP)
        {
            realpath(v[1], buf);

            v[1] = buf;
        }

        if (isDir(v[1]) || isFile(v[1]))
            strncpy(root_dir, v[1], sizeof(root_dir) - 1);
    }

    {
        size_t offset = strlen(root_dir);
        if (root_dir[offset] == PATH_SEP) root_dir[offset] = '\0';
    }

    spawnTreeThread(&global_tree, root_dir, strlen(root_dir), 2);

    int retval = 0;
    retval = app(c, v);

    if (tree_thread.joinable())
        tree_thread.join();

    freeTree(&global_tree);

    putLastPath(root_dir, sizeof(root_dir));


    return retval;
}
