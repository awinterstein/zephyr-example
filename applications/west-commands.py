'''west-commands.py

Basic example of a west extension.'''

from glob import glob
from multiprocessing import cpu_count  # for the number of threads
from queue import Queue
from textwrap import dedent            # just for nicer code indentation
from threading import Thread, Lock
from west.commands import WestCommand  # every extension must subclass this

import os
import subprocess
import sys

class ClangFormat(WestCommand):

    def __init__(self):
        super().__init__(
            'clang-format',  # gets stored as self.name
            'run clang-format on the source code',  # self.help
            # self.description:
            dedent('''
            Run clang-format on the source code of an application.

            The formatting is run on all header and implementation files of the module and
            violations can either be fixed or reported.'''))

    def do_add_parser(self, parser_adder):
        # This is a bit of boilerplate, which allows you full control over the
        # type of argparse handling you want. The "parser_adder" argument is
        # the return value of an argparse.ArgumentParser.add_subparsers() call.
        parser = parser_adder.add_parser(self.name,
                                         help=self.help,
                                         description=self.description)

        # Add some example options using the standard argparse module API.
        parser.add_argument('-f', '--fix', action='store_true',
                            help='fix the source code')
        parser.add_argument('-exclude-dirs', nargs='*', default=[],
                            help='directories (absolute path) to exclude from analysis.')
        parser.add_argument('source_dir', help='application source directory')

        return parser           # gets stored as self.parser

    def do_run(self, args, unknown_args):
        # Check whether clang-format can be called
        try:
            with open(os.devnull, 'w') as devnull:
                subprocess.check_call(['clang-format', '--dump-config'], stdout=devnull)
        except:
            print("Unable to run clang-format.", file=sys.stderr)
            sys.exit(1)

        # Check whether the source directory exists
        if not os.path.isdir(args.source_dir):
            print(f"Given source directory '{args.source_dir}' does not exist.", file=sys.stderr)
            sys.exit(1)

        # Find the source code files (*.h, *.c, *.hpp, *.cpp,)
        files = []
        files.extend(glob(args.source_dir + '/**/*.h', recursive=True))
        files.extend(glob(args.source_dir + '/**/*.c', recursive=True))
        files.extend(glob(args.source_dir + '/**/*.hpp', recursive=True))
        files.extend(glob(args.source_dir + '/**/*.cpp', recursive=True))

        return_code = 0
        try:
            # Spin up a bunch of format-launching threads
            task_queue = Queue(cpu_count())

            # List of files with a non-zero return code
            failed_files = []
            lock = Lock()
            for _ in range(task_queue.maxsize):
                t = Thread(target=ClangFormat.__run_format, args=(args, task_queue, lock, failed_files))
                t.daemon = True
                t.start()

            # Fill the queue with files.
            for name in files:
                name = name.replace("\\", "/")
                if not name.startswith(tuple(args.exclude_dirs)):
                    task_queue.put(name)

            # Wait for all threads to be done.
            task_queue.join()
            if len(failed_files):
              return_code = 1

        except KeyboardInterrupt:
            # This is a sad hack. Unfortunately subprocess goes
            # bonkers with ctrl-c and we start forking merrily.
            print('\nCtrl-C detected, goodbye.')
            os.kill(0, 9)

        sys.exit(return_code)


    @staticmethod
    def __run_format(args, queue, lock, failed_files):
        """Takes filenames out of queue and runs clang-format on them."""
        while True:
            name = queue.get()
            invocation = ClangFormat.__get_format_invocation(name, args.fix)

            proc = subprocess.Popen(invocation, stdout=subprocess.PIPE, stderr=subprocess.PIPE)
            output, err = proc.communicate()
            if proc.returncode != 0:
                failed_files.append(name)
            with lock:
                sys.stdout.write(' '.join(invocation) + '\n' + output.decode('utf-8'))
                if len(err) > 0:
                    sys.stdout.flush()
                    sys.stderr.write(err.decode('utf-8'))
                queue.task_done()

    @staticmethod
    def __get_format_invocation(f, fix):
        """Gets a command line for clang-format."""
        start = ['clang-format', '--Werror']
        if fix:
            start.append('-i')
        else:
            start.append('--dry-run')
        #if warnings_as_errors:
        #    start.append('--Werror')
        start.append(f)
        return start
