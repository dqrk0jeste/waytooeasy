project(
  'rasterizer',
  'c',
  version: '0.1.0',
  license: 'MIT',
)

wayland = dependency('wayland-client')
wayland_scanner = find_program('wayland-scanner')

wayland_protocols = dependency('wayland-protocols')
protocol_dir = wayland_protocols.get_variable('pkgdatadir')

protocol_files = [
  protocol_dir / 'stable/xdg-shell/xdg-shell.xml',
  # protocol_dir / 'unstable/xdg-output/xdg-output-unstable-v1.xml',
  # protocol_dir / 'staging/cursor-shape/cursor-shape-v1.xml',
  # 'protocols/wlr-layer-shell-unstable-v1.xml',
]

protocols = []
foreach f : protocol_files
  filename = f.split('/')[-1]
  # header files
  protocols += custom_target(
    output: filename.replace('.xml', '-protocol.h'),
    input: f,
    command: [
      wayland_scanner, 'client-header',
      '@INPUT@', '@OUTPUT@'
    ],
    build_by_default: true
  )

  # private code
  protocols += custom_target(
    output: filename.replace('.xml', '-protocol.c'),
    input: f,
    command: [
      wayland_scanner, 'private-code',
      '@INPUT@', '@OUTPUT@'
    ],
    build_by_default: true
  )
endforeach

deps = [
    dependency('wayland-client'),
]

src = [
    'app.c',
    'memory.c',
    'buffer.c',
    'color.c',
    'event_loop.c',
    'mesh.c',
    'scene.c',
    'camera.c',
    'wayland.c',
    'triangle.c',
]

inc = [
    '.',
    'util',
    'build',
]

executable(
    'main',
    src,
    protocols,
    dependencies: deps,
    include_directories: include_directories(inc),
    link_args : '-lrt',
)

# install_data('LICENSE', install_dir: get_option('datadir') / 'licenses/language')
