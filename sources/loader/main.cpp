#include "application.h"

int main(int argc, char *argv[])
{
    CApplication &application = CApplication::GetInstance();
    return application.Run(argc, argv);
}
