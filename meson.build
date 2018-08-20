project('mandexing', 'cpp')
qt5 = import('qt5')
qt5_dep = dependency('qt5', modules: ['Core', 'Gui', 'Widgets'])
png_dep = dependency('libpng')

moc_files = qt5.preprocess(moc_headers : ['Dialogue.h', 'PredictionView.h', 'Tinker.h'],
                           moc_extra_arguments: ['-DMAKES_MY_MOC_HEADER_COMPILE'])

executable('mandexing', 'Crystal.cpp', 'CSV.cpp', 'Detector.cpp', 'Dialogue.cpp', 'FileReader.cpp', 'main.cpp', 'mat3x3.cpp', 'Node.cpp', 'PNGFile.cpp', 'PredictionView.cpp', 'RefinementGridSearch.cpp', 'RefinementNelderMead.cpp', 'RefinementStepSearch.cpp', 'RefinementStrategy.cpp', 'TextManager.cpp', 'Tinker.cpp', 'vec3.cpp', moc_files, dependencies: [qt5_dep, png_dep])

#

