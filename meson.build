# Root meson.build file for lotos-screensaver project

project(
  'lotos-screensaver',
  'cpp',
  version: '1.0.0',
  meson_version: '>= 0.55.0',
  default_options : ['cpp_std=c++20']
)
add_project_arguments('-std=c++20', language : 'cpp')

# Define common include directory
common_inc = include_directories('screensaver/common')

# Screensaver executable
screensaver_dep = dependency('sdl2', required: true)
mpv_dep = dependency('mpv', required: true)
gl_dep = dependency('gl', required: true)
glew_dep = dependency('glew', required: true)
jsocpp_dep = dependency('jsoncpp', required: true)
sdl2_ttf_dep = dependency('sdl2_ttf', required: true)
spdlog_dep = dependency('spdlog', required: true)

executable('lotos-screensaver',
  'screensaver/screensaver/screensaver.cpp',
  include_directories: common_inc,
  dependencies: [screensaver_dep, mpv_dep, gl_dep, glew_dep, sdl2_ttf_dep, jsocpp_dep, spdlog_dep],
  install: true,
  install_dir: '/usr/bin'
)
