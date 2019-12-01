#include <memory>
#include <getopt.h>
#include <xeus/xkernel.hpp>
#include <xeus/xkernel_configuration.hpp>

#include "xinterpreter.hpp"

using namespace xfift;

#ifdef XFUNC_KERNEL
    #define STARTING "Starting xeus-func kernel...\n\n"
    typedef xfift::interpreter<xfift::XFunc> xint_impl;
#else
    #define STARTING "Starting xeus-fift kernel...\n\n"
    typedef xfift::interpreter<xfift::XFift> xint_impl;
#endif

std::string extract_filename(int& argc, char* argv[])
{
    std::string res = "";
    for (int i = 0; i < argc; ++i)
    {
        if ((std::string(argv[i]) == "-f") && (i + 1 < argc))
        {
            res = argv[i + 1];
            for(int j = i; j < argc - 2; ++j)
            {
                argv[j]  = argv[j + 2];
            }
            argc -= 2;
            break;
        }
    }
    return res;
}

int main(int argc, char* argv[]) {
    std::string file_name = extract_filename(argc, argv);

    using interpreter_ptr = std::unique_ptr<xint_impl>;
    interpreter_ptr interpreter = interpreter_ptr(new xint_impl());

    if (!file_name.empty()) {
        xeus::xconfiguration config = xeus::load_configuration(file_name);
        xeus::xkernel kernel(config, xeus::get_user_name(), std::move(interpreter));
        
        std::clog <<
            STARTING
            "If you want to connect to this kernel from an other client, you can use"
            " the " + file_name + " file.\n";

        kernel.start();
    } else {
        xeus::xkernel kernel(xeus::get_user_name(), std::move(interpreter));

        const auto& config = kernel.get_config();
        std::clog <<
            STARTING
            "If you want to connect to this kernel from an other client, just copy"
            " and paste the following content inside of a `kernel.json` file. And then run for example:\n\n"
            "# jupyter console --existing kernel.json\n\n"
            "kernel.json\n```\n{\n"
            "    \"transport\": \"" + config.m_transport + "\",\n"
            "    \"ip\": \"" + config.m_ip + "\",\n"
            "    \"control_port\": " + config.m_control_port + ",\n"
            "    \"shell_port\": " + config.m_shell_port + ",\n"
            "    \"stdin_port\": " + config.m_stdin_port + ",\n"
            "    \"iopub_port\": " + config.m_iopub_port + ",\n"
            "    \"hb_port\": " + config.m_hb_port + ",\n"
            "    \"signature_scheme\": \"" + config.m_signature_scheme + "\",\n"
            "    \"key\": \"" + config.m_key + "\"\n"
            "}\n```\n";

        kernel.start();
    }

    return 0;
}