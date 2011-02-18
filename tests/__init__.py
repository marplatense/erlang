# utility functions

from decimal import Decimal, InvalidOperation

def to_decimal(value):
    """Convert a string to Decimal"""
    if value=='' or value is None:
        return None
    else:
        try:
            return Decimal(value)
        except InvalidOperation:
            return value

