import sys
import json
import os

print('Running "' + sys.argv[0] + '"')

if len(sys.argv) < 3:
	print('Invalid parameters to "' + sys.argv[0] + '"')
	sys.exit(1)

print('Reading "' + sys.argv[1] + '"')

BUILD_MAJOR = '0'
BUILD_MINOR = '0'
BUILD_PATCH = '0'
BUILD_NUMBER = os.environ.get("BUILD_NUMBER", "0")
BUILD_BRANCH = os.environ.get('JOB_NAME', 'Unknown')
with open(sys.argv[1]) as version_json:
	data = json.load(version_json)
	BUILD_MAJOR = data['major']
	BUILD_MINOR = data['minor']
	BUILD_PATCH = data['patch']

NFDC_BUILD_NUMBER = BUILD_MAJOR + '.' + BUILD_MINOR + '.' + BUILD_PATCH + '.' + BUILD_NUMBER

print('Writing "' + sys.argv[2] + '"')
header = open(sys.argv[2], 'w')
header.write('// File created by Python script "' + sys.argv[0] + '"\n')
header.write('// Version numbers have been taken from "' + sys.argv[1] + '"\n\n')
header.write('#pragma once\n\n')
header.write('#ifndef _NFDCVERSIONS_H_\n')
header.write('#define _NFDCVERSIONS_H_\n\n')
header.write('#define NFDC_BUILD_MAJOR "' + BUILD_MAJOR + '"\n')
header.write('#define NFDC_BUILD_MINOR "' + BUILD_MINOR + '"\n')
header.write('#define NFDC_BUILD_PATCH "' + BUILD_PATCH + '"\n')
header.write('#define NFDC_BUILD_NUMBER "' + NFDC_BUILD_NUMBER + '"\n')
header.write('#define NFDC_BUILD_BRANCH "' + BUILD_BRANCH + '"\n')
header.write('\n#endif // _NFDCVERSIONS_H_\n')
header.close()

bat_name = sys.argv[2]
bat_name = bat_name.replace(".h", ".bat")

print('Writing "' + bat_name + '"')
bat = open(bat_name, 'w')
bat.write(':: File created by Python script "' + sys.argv[0] + '"\n')
bat.write(':: Version numbers have been taken from "' + sys.argv[1] + '"\n\n')
bat.write('set NFDC_BUILD_MAJOR=' + BUILD_MAJOR + '\n')
bat.write('set NFDC_BUILD_MINOR=' + BUILD_MINOR + '\n')
bat.write('set NFDC_BUILD_PATCH=' + BUILD_PATCH + '\n')
bat.write('set NFDC_BUILD_NUMBER=' + NFDC_BUILD_NUMBER + '\n')
bat.write('set NFDC_BUILD_BRANCH=' + BUILD_BRANCH + '\n')
bat.close()
