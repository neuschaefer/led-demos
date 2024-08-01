project('led-demos', 'cpp')

compiler = meson.get_compiler('cpp')
rgbmatrix = compiler.find_library('rgbmatrix', has_headers: ['led-matrix.h'])

wanja     = executable('wanja',     'wanja.cc',     dependencies: rgbmatrix, install: true)
marcel    = executable('marcel',    'marcel.cc',    dependencies: rgbmatrix, install: true)
best      = executable('best',      'best.cc',      dependencies: rgbmatrix, install: true)
rainbow   = executable('rainbow',   'rainbow.cc',   dependencies: rgbmatrix, install: true)
mimoja    = executable('mimoja',    'mimoja.cc',    dependencies: rgbmatrix, install: true)
ledflut   = executable('ledflut',   'ledflut.cc',   dependencies: rgbmatrix, install: true)
fireworks = executable('fireworks', 'fireworks.cc', dependencies: rgbmatrix, install: true)
