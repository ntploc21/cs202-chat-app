#include "ClientLayer.hpp"
#include "Walnut/ApplicationGUI.h"
#include "Walnut/EntryPoint.h"

Walnut::Application* Walnut::CreateApplication(int argc, char** argv) {
    Walnut::ApplicationSpecification spec;
    spec.Name = "Loc Chat";
    // spec.IconPath = "???";
    spec.CustomTitlebar = true;
    spec.CenterWindow = true;

    Walnut::Application* app = new Walnut::Application(spec);
    std::shared_ptr< ClientLayer > client_layer =
        std::make_shared< ClientLayer >();
    app->PushLayer(client_layer);
    app->SetMenubarCallback([app, client_layer]() {
        if (ImGui::BeginMenu("File")) {
            /*if (ImGui::MenuItem("Disconnect"))
                client_layer->OnDisconnectButton();*/

            if (ImGui::MenuItem("Exit")) app->Close();
            ImGui::EndMenu();
        }
    });
    return app;
}