import csv
from decimal import Decimal
try:
    from collections import OrderedDict
except ImportError:
    from ordereddict import OrderedDict

from nose.tools import *

from erlang import *
from tests import to_decimal

tables = ['tests/TableA.csv', 'tests/TableE_a.csv', 'tests/TableE_b.csv']

@nottest
def build_arrays(table):
    f = open(table, 'r')
    r = csv.reader(f)
    a = r.next()
    k = [to_decimal(i) for i in a[1:]]
    try:
        while 1:
            a = r.next()
            k1, v = a[0], [to_decimal(i) for i in a[1:]] 
            yield(k1, OrderedDict(zip(k,v)))
    finally:
        f.close()

def test_A_erlangb():
    """Basic erlang calculation"""
    exp = Decimal('0.0522')
    eq_(erlang_b(6, 3).quantize(exp), exp)

def test_B_erlangb_neg():
    """Negative values always return 0"""
    eq_(erlang_b(-6,3), Decimal('0'))
    eq_(erlang_b(6,-3), Decimal('0'))
    eq_(erlang_b(-6,-3), Decimal('0'))

def test_C_tableA():
    input = build_arrays('tests/TableA.csv')
    # we might generate too many data and we need to inspect the results, so we
    # better put it in a file
    result_f = open('tests/test_tableA_results', 'w')
    result_d = OrderedDict()
    ok___ = 0
    wrong = 0
    try:
        while 1:
            l = input.next()
            for k, v in l[1].items():
                try:
                    res = erlang_b(l[0], k)
                except Exception, e:
                    res = e.args[0]
                if res == v: ok___ += 1
                else: wrong += 1
                result_d[(l[0], k)] = "Expected output: %r, Got: %r" % (v, res)
    except StopIteration:
        result_f.write("Ok results: %d\n" % ok___)
        result_f.write("Wrong results: %d\n" % wrong)
        result_f.write("Ratio: %s\n" % (float(ok___)/float(ok___+wrong)*100))
        for k, v in result_d.items():
            result_f.write("%r, %r: %s\n" % (k[0], k[1], v))
        result_f.close()
    ok_(wrong==0, "There were errors, please check tests/test_tableA_results")


