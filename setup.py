from distutils.core import setup, Extension

erlang = Extension('erlang',
                   sources = ['erlang/erlangmodule.c'])

setup (name = 'Erlang',
       version = '0.1',
       description = 'This is a demo package',
       author = 'Mariano Mara',
       author_email = 'mariano.mara@gmail.com',
       long_description = '''
Erlang B
''',
       ext_modules = [erlang])


