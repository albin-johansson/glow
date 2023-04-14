#include "command_line_parser.hpp"

#include <exception>  // exception

#include <argparse/argparse.hpp>

#include "common/predef.hpp"
#include "common/type/map.hpp"
#include "common/type/string.hpp"

namespace glow {
namespace {

inline constexpr const char* kApiHelp = "graphics API that will be used";
inline constexpr const char* kEnvHelp = "path to environment texture to load";
inline constexpr const char* kLogHelp = "verbosity of log output";
inline constexpr const char* kModelsHelp = "list of model files to load";

inline constexpr const char* kEpilog =
    "Supported graphics APIs: 'OpenGL', 'Vulkan'\n"
    "Supported log levels: [0, 6]";

inline constexpr StringView kDefaultApi = "OpenGL";
inline constexpr int kDefaultLogLevel = 4;

inline const Map<StringView, GraphicsApi> kSupportedAPIs {
    {"OpenGL", GraphicsApi::OpenGL},
    {"Vulkan", GraphicsApi::Vulkan},
};

inline const HashMap<int, LogLevel> kLogLevels {
    {0, LogLevel::off},
    {1, LogLevel::critical},
    {2, LogLevel::err},
    {3, LogLevel::warn},
    {4, LogLevel::info},
    {5, LogLevel::debug},
    {6, LogLevel::trace},
};

}  // namespace

auto parse_command_line_args(const int argc, char** argv) -> Maybe<CommandLineArgs>
{
  argparse::ArgumentParser parser {"glow",
                                   GRAVEL_VERSION_STRING,
                                   argparse::default_arguments::all};

  // clang-format off
  parser.add_argument("--api", "-a").nargs(1).default_value(kDefaultApi).help(kApiHelp);
  parser.add_argument("--env", "-e").nargs(1).help(kEnvHelp);
  parser.add_argument("--models", "-m").nargs(argparse::nargs_pattern::any).help(kModelsHelp);
  parser.add_argument("--log", "-l").nargs(1).scan<'i', int>().default_value(kDefaultLogLevel).help(kLogHelp);
  parser.add_epilog(kEpilog);
  // clang-format on

  try {
    parser.parse_args(argc, argv);
  }
  catch (const std::exception& e) {
    spdlog::error("[IO] Invalid command line arguments: {}", e.what());
    return kNothing;
  }
  catch (...) {
    spdlog::error("[IO] Invalid command line arguments");
    return kNothing;
  }

  CommandLineArgs args;

  if (parser.is_used("--api")) {
    const auto& api = parser.get<String>("--api");

    if (const auto iter = kSupportedAPIs.find(api); iter != kSupportedAPIs.end()) {
      args.api = iter->second;
    }
    else {
      spdlog::warn("[IO] Unsupported graphics API option '{}'", api);
    }
  }

  if (parser.is_used("--env")) {
    args.env_path = parser.get<String>("--env");
  }

  if (parser.is_used("--models")) {
    const auto models = parser.get<Vector<String>>("--models");
    args.model_paths.reserve(models.size());

    for (const auto& model_path : models) {
      args.model_paths.emplace_back(model_path);
    }
  }

  if (parser.is_used("--log")) {
    const auto level = parser.get<int>("--log");

    if (const auto iter = kLogLevels.find(level); iter != kLogLevels.end()) {
      args.log_level = iter->second;
    }
    else {
      spdlog::warn("[IO] Invalid log level '{}'", level);
    }
  }

  return args;
}

}  // namespace glow
