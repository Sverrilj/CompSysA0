#!/usr/bin/env bash

# Exit immediately if any command below fails.
set -e

# Compile the file.c program (assuming Makefile exists)
make iso

echo "Generating a test_files directory.."
mkdir -p test_files
rm -f test_files/*

echo "Generating test files.."

# ASCII files
printf "Hello, World!\n" > test_files/ascii1.input
printf "Another line of ASCII text\n" > test_files/ascii2.input
printf "Just an ASCII test\n" > test_files/ascii3.input

# ISO-8859-1 files (non-ASCII characters)
printf "ISO-8859-1: \xA9 \xAE \xF1\n" > test_files/iso8859_1.input
printf "ISO-8859-1: \xB5 \xBF \xD7\n" > test_files/iso8859_2.input
printf "ISO-8859-1: \xA1 \xA3 \xBD\n" > test_files/iso8859_3.input

# UTF-8 files (with multibyte characters)
printf "UTF-8: \xC3\xA9 \xC3\xB1 \xC3\xBC\n" > test_files/utf8_1.input  # é, ñ, ü
printf "UTF-8: \xE2\x82\xAC \xF0\x9F\x98\x81\n" > test_files/utf8_2.input  # €, 😁
printf "UTF-8: \xE2\x9C\x85 \xE2\x99\xA5\n" > test_files/utf8_3.input  # ✅, ♥

# Binary data files (non-text files)
head -c 20 /dev/urandom > test_files/binary1.input
head -c 50 /dev/urandom > test_files/binary2.input
head -c 100 /dev/urandom > test_files/binary3.input

# Empty files
touch test_files/empty1.input
touch test_files/empty2.input

# Test permission error (create a file and remove read permission)
touch test_files/permission_denied.input
chmod -r test_files/permission_denied.input

echo "Running the tests.."
exitcode=0
for f in test_files/*.input
do
  echo ">>> Testing ${f}.."

  # Use the system's file command for the expected output and process it
  file    ${f} | sed -e 's/ASCII text.*/ASCII text/' \
                         -e 's/UTF-8 Unicode text.*/UTF-8 Unicode text/' \
                         -e 's/ISO-8859 text.*/ISO-8859 text/' \
                         -e 's/data.*/data/' \
                         -e 's/writable, regular file, no read permission/cannot determine (Permission denied)/' \
                         > "${f}.expected"

  # Run your own implementation to generate actual output
  ./iso  "${f}" > "${f}.actual"

  # Compare the expected and actual outputs
  if ! diff -u "${f}.expected" "${f}.actual"
  then
    echo ">>> Failed :-("
    exitcode=1
  else
    echo ">>> Success :-)"
  fi
done
exit $exitcode


