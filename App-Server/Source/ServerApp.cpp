#include <iostream>

#include "ServerLayer.hpp"
#include "Walnut/Application.h"
#include "Walnut/EntryPoint.h"

Walnut::Application* Walnut::CreateApplication(int argc, char** argv) {
    Walnut::ApplicationSpecification spec;
    spec.Name = "Loc Chat Server 1.0";

    Walnut::Application* app = new Walnut::Application(spec);
    app->PushLayer< ServerLayer >();
#ifndef WL_HEADLESS
    app->SetMenubarCallback([app]() {
        if (ImGui::BeginMenu("File")) {
            if (ImGui::MenuItem("Exit")) {
                app->Close();
            }
            ImGui::EndMenu();
        }
    });
#endif
    return app;
}