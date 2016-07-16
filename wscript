#############################################################################
## Crytek Source File
## Copyright (C) 2013, Crytek Studios
##
## Creator: Christopher Bolte
## Date: Oct 31, 2013
## Description: WAF  based build system
#############################################################################

def build(bld):

	bld.CryEngineModule(
		target        = 'Chrysalis',
		vs_filter     = 'Projects/Chrysalis',
		file_list     = 'chrysalis.waf_files',
		pch           = 'StdAfx.cpp',
		includes      = [ '.',  Path('Code/CryEngine/CryAction')],
		win_lib       = [ 'shell32', 'Gdi32' ],
	)