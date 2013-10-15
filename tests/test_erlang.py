import csv
from decimal import Decimal
from collections import OrderedDict

from nose.tools import *

from erlang import *
from tests import to_decimal

tables = ['tests/TableA.csv', 'tests/Table_ExtErlangB_40retry.csv',
          'tests/Table_ExtErlangB_50retry.csv']

@nottest
def build_arrays(table):
    f = open(table, 'r')
    r = csv.reader(f)
    a = next(r)
    k = [to_decimal(i) for i in a[1:]]
    try:
        while 1:
            a = next(r)
            k1, v = a[0], [to_decimal(i) for i in a[1:]]
            yield(k1, OrderedDict(list(zip(k,v))))
    finally:
        f.close()

def test_AA_erlang_b():
    """Basic erlang calculation"""
    exp = Decimal('0.0522')
    eq_(erlang_b(6, 3).quantize(exp), exp)

def test_AB_erlang_b_ext():
    """Basic erlang calculation"""
    exp = Decimal('0.0721')
    eq_(erlang_b_ext(6, 3, Decimal('0.5')).quantize(exp), exp)
    exp = Decimal('0.3')
    eq_(erlang_b_ext(2, Decimal('1.2744'), Decimal('0.4')).quantize(exp), exp)

def test_B_neg():
    """Negative values always return 0"""
    eq_(erlang_b(-6,3), Decimal('1'))
    eq_(erlang_b(6,-3), Decimal('0'))
    eq_(erlang_b(-6,-3), Decimal('0'))
    eq_(erlang_b(Decimal("-6"),Decimal("3")), Decimal('1'))
    eq_(erlang_b(Decimal("6"),Decimal("-3")), Decimal('0'))
    eq_(erlang_b(Decimal("-6"),Decimal("-3")), Decimal('0'))

def test_C_float():
    """What happens if we pass floats?"""
    exp = Decimal('0.0522')
    eq_(erlang_b(float(6.0), float(3.0)).quantize(exp), exp)

def test_D_erlangB():
    """We test the erlang b function"""
    input = build_arrays('tests/TableA.csv')
    # we might generate too much data and we need to inspect the results, so we
    # better put it in a file
    result_f = open('tests/test_TableA_results', 'w')
    result_d = OrderedDict()
    ok___ = 0
    wrong = 0
    try:
        while 1:
            l = next(input)
            for k, v in list(l[1].items()):
                try:
                    res = erlang_b(Decimal(l[0]), v)
                except Exception as e:
                    res = e.args[0]
                if res.quantize(k) == k:
                    ok___ += 1
                else:
                    wrong += 1
                result_d[(l[0], v)] = "Expected output: %r, Got: %r" % (k, res.quantize(k))
    except StopIteration:
        result_f.write("Ok results: %d\n" % ok___)
        result_f.write("Wrong results: %d\n" % wrong)
        result_f.write("Ratio: %s\n" % (float(ok___)/float(ok___+wrong)*100))
        for k, v in list(result_d.items()):
            result_f.write("%r, %r: %s\n" % (k[0], k[1], v))
        result_f.close()
    ok_(wrong==0, "There were errors, please check tests/test_TableA_results")

def test_E_extended_erlangB():
    """We test the extended erlang b function"""
    input = build_arrays('tests/Table_ExtErlangB_40retry.csv')
    # we might generate too much data and we need to inspect the results, so we
    # better put it in a file
    result_f = open('tests/test_Table_ExtErlangB_results', 'w')
    result_d = OrderedDict()
    ok___ = 0
    wrong = 0
    global_wrong = 0
    try:
        while 1:
            l = next(input)
            for k, v in list(l[1].items()):
                try:
                    res = erlang_b_ext(Decimal(l[0]), v, Decimal('.4'))
                except Exception as e:
                    res = e.args[0]
                if res.quantize(k) == k: ok___ += 1
                else:
                    wrong += 1
                result_d[(l[0], v)] = "Expected output: %r, Got: %r" % (k, res.quantize(k))
    except StopIteration:
        result_f.write("============ 40% retry ============\n")
        result_f.write("Ok results: %d\n" % ok___)
        result_f.write("Wrong results: %d\n" % wrong)
        result_f.write("Ratio: %s\n" % (float(ok___)/float(ok___+wrong)*100))
        for k, v in list(result_d.items()):
            result_f.write("%r, %r: %s\n" % (k[0], k[1], v))
    global_wrong += wrong
    input = build_arrays('tests/Table_ExtErlangB_50retry.csv')
    result_d = OrderedDict()
    ok___ = 0
    wrong = 0
    try:
        while 1:
            l = next(input)
            for k, v in list(l[1].items()):
                try:
                    res = erlang_b_ext(Decimal(l[0]), v, Decimal('.5'))
                except Exception as e:
                    res = e.args[0]
                if res.quantize(k) == k: ok___ += 1
                else: wrong += 1
                result_d[(l[0], v)] = "Expected output: %r, Got: %r" % (k, res.quantize(k))
    except StopIteration:
        result_f.write("============ 50% retry ============\n")
        result_f.write("Ok results: %d\n" % ok___)
        result_f.write("Wrong results: %d\n" % wrong)
        result_f.write("Ratio: %s\n" % (float(ok___)/float(ok___+wrong)*100))
        for k, v in list(result_d.items()):
            result_f.write("%r, %r: %s\n" % (k[0], k[1], v))
        result_f.close()
    global_wrong += wrong
    ok_(global_wrong==0, "There were errors, please check tests/test_Table_ExtErlangB_results")
