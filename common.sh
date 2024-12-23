#=======================================================================================================================
# Shared library of shell functions
#=======================================================================================================================

# Coloring reference, from https://tldp.org/HOWTO/Bash-Prompt-HOWTO/x329.html
# Non-printing escape sequences have to be enclosed in \[\033[ and \]. For colour escape sequences, they should also be
# followed by a lowercase m:
#
# Black         0;30
# Red           0;31
# Green         0;32
# Brown         0;33
# Blue          0;34
# Purple        0;35
# Cyan          0;36
# Light Gray    0;37
# Dark Gray     1;30
# Light Red     1;31
# Light Green   1;32
# Yellow        1;33
# Light Blue    1;34
# Light Purple  1;35
# Light Cyan    1;36
# White         1;37

CLR_DATE=$'\E[01;37m'
CLR_VAR=$'\E[01;36m'
CLR_OK=$'\E[01;32m'
CLR_WARN=$'\E[01;33m'
CLR_ERR=$'\E[01;31m'
CLR_OFF=$'\E[0m'
unset LOG_FILE

#-----------------------------------------------------------------------------------------------------------------------
# Common functions
#-----------------------------------------------------------------------------------------------------------------------

# Logs a message to screen, and, optionally, to $LOG_FILE (if it's set)
# Arguments:
#   $1 message
#   $2 optional, uncoloured message
log() {
    date="$(date +%T)"
    echo -e "${CLR_DATE}${date}${CLR_OFF} $1"
    [[ -n "$LOG_FILE" ]] && echo "$date ${2:-$1}" >> "$LOG_FILE"
}

# Logs a normal message
# Arguments:
#   $1 message
info() {
    log "i $1" "$2"
}

# Logs a success message
# Arguments:
#   $1 message
ok() {
    log "${CLR_OK}+ $1${CLR_OFF}" "+ $1"
}

# Logs a warning message
# Arguments:
#   $1 message
warn() {
    log "${CLR_WARN}WARNING: $1${CLR_OFF}" "WARNING: $1"
}

# Logs a failure message and exits
# Arguments:
#   $1 message
err() {
    log "${CLR_ERR}ERROR: $1${CLR_OFF}" "ERROR: $1"
    exit 2
}

# Same as err(), but doesn't exit
# Arguments:
#   $1 message
error() {
    log "${CLR_ERR}ERROR: $1${CLR_OFF}" "ERROR: $1"
}

# Displays usage info (taken from the mandatory var USAGE_INFO) and exits
# Arguments:
#   $1 optional error message
usage() {
  [[ -z "$USAGE_INFO" ]] && echo "FATAL: USAGE_INFO isn't defined" >&2 && exit 9
  [[ -n "$1" ]] && log "${CLR_ERR}ERROR: $1${CLR_OFF}" "ERROR: $1"
  echo "$USAGE_INFO" >&2
  exit 1
}

# Handles Ctrl-C
interrupted() {
    warn "Processing interrupted"
    exit 10
}
