#include "mc.h"
#include "Application.h"

int main()
{
    Application& app = Application::Get();
    app.Run();

    
    return 0;
}