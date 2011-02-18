# -*- coding: utf-8 -*-
"""
Several probability distribution formulas for traffic engineering: ErlangB,
Extended Erlang B, Equivalent Random Theory, Engset, Binomial and Erlang C

Bibliography: 
    * Traffic Engineering Techniques in Telecommunications by Richard Parkinson
      © Copyright INFOTEL SYSTEMS CORP.
    * Extract from the Table of the Erlang Loss Formula 
      Edited by Mr. H. Leijon, ITU
      http://www.itu.int/itudoc/itu-d/dept/psp/ssb/planitu/plandoc/erlangt.html
    * Wikipedia entry for ErlangB (http://en.wikipedia.org/wiki/Erlang_B)  

"""
from decimal import Decimal

__all__ = ['erlang_b', 'extended_erlang_b', 'equivalent_random_theory',
           'engset', 'binomial', 'erlang_c']

def erlang_b(servers, traffic):
    """Erlang-B is a formula for the blocking probability derived from the 
    Erlang distribution to describe the probability of call loss on a group of
    circuits.
    Parameters:
        servers: number of servers (lines)
        traffic: offered traffic
    returns:
        probability of blocking
    """
    if servers <0 or traffic<0:
        return Decimal(0)
    s = Decimal(1)
    for i in range(1, servers+1):
        r = (traffic*s)/(i+(traffic*s))
        s = r
    return r

def extended_erlang_b(servers, traffic, retry):
    """TODO: add doc"""
    return NotImplemented

def equivalent_random_theory():
    """TODO: add doc"""
    return NotImplemented

def engset(servers, traffic, sources):
    """TODO: add doc"""
    return NotImplemented

def binomial(servers, traffic, sources):
    """TODO: add doc"""
    return NotImplemented

def erlang_c(servers, traffic):
    """TODO: add doc"""
    return NotImplemented


