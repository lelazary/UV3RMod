# srecutils.py
#
# Copyright (C) 2011 Gabriel Tremblay - initnull hat gmail.com
#
# This program is free software; you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation; either version 2 of the License, or
# (at your option) any later version.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with this program; if not, write to the Free Software
# Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA

"""
Motorola S-Record utis
- Kudos to Montreal CISSP Groupies
"""

# Address len in bytes for S* types
# http://www.amelek.gda.pl/avr/uisp/srecord.htm
__ADDR_LEN = {'S0' : 2,
              'S1' : 2,
              'S2' : 3,
              'S3' : 4,
              'S5' : 2,
              'S7' : 4,
              'S8' : 3,
              'S9' : 2}


def int_to_padded_hex_byte(integer):
    """
Convert an int to a 0-padded hex byte string
example: 65 == 41, 10 == 0A
Returns: The hex byte as string (ex: "0C")
"""
    to_hex = hex(integer)
    xpos = to_hex.find('x')
    hex_byte = to_hex[xpos+1 : len(to_hex)].upper()

    if len(hex_byte) == 1:
        hex_byte = ''.join(['0', hex_byte])

    return hex_byte


def compute_srec_checksum(srec):
    """
Compute the checksum byte of a given S-Record
Returns: The checksum as a string hex byte (ex: "0C")
"""
    # Get the summable data from srec
    # start at 2 to remove the S* record entry
    data = srec[2:len(srec)]

    sum = 0
    # For each byte, convert to int and add.
    # (step each two character to form a byte)
    for position in range(0, len(data), 2):
        current_byte = data[position : position+2]
        int_value = int(current_byte, 16)
        sum += int_value

    # Extract the Least significant byte from the hex form
    hex_sum = hex(sum)
    least_significant_byte = hex_sum[len(hex_sum)-2:]
    least_significant_byte = least_significant_byte.replace('x', '0')

    # turn back to int and find the 8-bit one's complement
    int_lsb = int(least_significant_byte, 16)
    computed_checksum = (~int_lsb) & 0xff

    return computed_checksum


def validate_srec_checksum(srec):
    """
Validate if the checksum of the supplied s-record is valid
Returns: True if valid, False if not
"""
    checksum = srec[len(srec)-2:]

    # Strip the original checksum and compare with the computed one
    if compute_srec_checksum(srec[:len(srec) - 2]) == int(checksum, 16):
        return True
    else:
        return False


def get_readable_string(integer):
    r"""
Convert an integer to a readable 2-character representation. This is useful for reversing
examples: 41 == ".A", 13 == "\n", 20 (space) == "__"
Returns a readable 2-char representation of an int.
"""
    if integer == 9: #\t
        readable_string = "\\t"
    elif integer == 10: #\r
        readable_string = "\\r"
    elif integer == 13: #\n
        readable_string = "\\n"
    elif integer == 32: # space
        readable_string = '__'
    elif integer >= 33 and integer <= 126: # Readable ascii
        readable_string = ''.join([chr(integer), '.'])
    else: # rest
        readable_string = int_to_padded_hex_byte(integer)

    return readable_string


def offset_byte_in_data(target_data, offset, target_byte_pos, readable = False, wraparound = False):
    """
Offset a given byte in the provided data payload (kind of rot(x))
readable will return a human-readable representation of the byte+offset
wraparound will wrap around 255 to 0 (ex: 257 = 2)
Returns: the offseted byte
"""
    byte_pos = target_byte_pos * 2
    prefix = target_data[:byte_pos]
    suffix = target_data[byte_pos+2:]
    target_byte = target_data[byte_pos:byte_pos+2]
    int_value = int(target_byte, 16)
    int_value += offset

    # Wraparound
    if int_value > 255 and wraparound:
        int_value -= 256

    # Extract readable char for analysis
    if readable:
        if int_value < 256 and int_value > 0:
            offset_byte = get_readable_string(int_value)
        else:
            offset_byte = int_to_padded_hex_byte(int_value)
    else:
        offset_byte = int_to_padded_hex_byte(int_value)

    return ''.join([prefix, offset_byte, suffix])



# offset can be from -255 to 255
def offset_data(data_section, offset, readable = False, wraparound = False):
    """
Offset the whole data section.
see offset_byte_in_data for more information
Returns: the entire data section + offset on each byte
"""
    for pos in range(0, len(data_section)/2):
        data_section = offset_byte_in_data(data_section, offset, pos, readable, wraparound)

    return data_section


def parse_srec(srec):
    """
Extract the data portion of a given S-Record (without checksum)
Returns: the record type, the lenght of the data section, the write address, the data itself and the checksum
"""
    record_type = srec[0:2]
    data_len = srec[2:4]
    addr_len = __ADDR_LEN.get(record_type) * 2
    addr = srec[4:4 + addr_len]
    data = srec[4 + addr_len:len(srec)-2]
    checksum = srec[len(srec) - 2:]
    return record_type, data_len, addr, data, checksum
