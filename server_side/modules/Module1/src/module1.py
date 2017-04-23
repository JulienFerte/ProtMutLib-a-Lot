#!/usr/bin/env python
# encoding: utf-8

import sys
import cStringIO

save_stdout = sys.stdout

discard = cStringIO.StringIO()
sys.stdout = discard

# import stuff

# make computations
result_a = sys.argv[1]

# make computations as well
result_b = sys.argv[1]

result = cStringIO.StringIO()
sys.stdout = result

sys.stdout.write('#')
sys.stdout.write( str( result_a ) )
sys.stdout.write('#')
sys.stdout.write( str( result_b ) )
sys.stdout.write('#')

sys.stdout = save_stdout

sys.stdout.write( result.getvalue() )

