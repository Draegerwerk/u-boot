package require ::quartus::project
set need_to_close_project 0
if {[project_exists m48_default_system]} {
		puts ""
	} else {
		project_new -revision m48_default_system m48_default_system
		set need_to_close_project 1
	}

if {$need_to_close_project} {
	project_close
	}

