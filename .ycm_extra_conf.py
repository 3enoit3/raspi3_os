
def FlagsForFile( filename, **kwargs ):
    # CPPFLAGS = -Wall -O2 -ffreestanding -nostdinc++ -nostdlib -nostartfiles
    return {
        'flags': [ '-x', 'c++', '-Wall', '-Wextra', '-Werror', '-ffreestanding', '-nostdinc++', '-nostdlib', '-nostartfiles'],
    }

