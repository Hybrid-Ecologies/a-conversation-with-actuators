# MIDI CONTROLLER
# =================
# Controls the logic of the Launchpad MIDI Controller
# 
# PAD LOGIC
# Each pad can store a timesignal; knobs above the pad control time signal duration
# Pressing a pad activates the timesignal
# 
# CURSOR LOGIC
# Cursors are used to toggle between different stage interaction modes. 
# 

lastSignalSelect = null
SIGNAL_SELECT_SENSITIVITY = 20
last_track_val = [null, null, null, null, null, null, null, null]
window.orig_signal = [null, null, null, null, null, null, null, null]
$ ->
  window.ctrl = new LaunchControl()

  # RESET ALl PADS
  ctrl.open().then ->
    alertify.notify("<b> MIDI CHOREOGRAPHER CONNECTED </b> <br>Touch a pad to store a signal.")    
    ctrl.dirty_reset = true

  ctrl.on 'message', (e) ->
    if ctrl.dirty_reset
      defaultMIDI()
      ctrl.dirty_reset = false
    console.log 'dataType: ' + e.dataType
    console.log 'track   : ' + e.track
    console.log 'value   : ' + e.value
    console.log 'channel : ' + e.channel
    console.log 'direction : ' + e.direction
    switch e.dataType
      when "knob1"
        p = e.value / 128

        if e.track == 7
          if not lastSignalSelect then lastSignalSelect = e.value
          diff = (e.value - lastSignalSelect) % SIGNAL_SELECT_SENSITIVITY
          if Math.abs(diff) > 0
            signalSelectBehavior(e, diff)
            lastSignalSelect = e.value
        if id = theatre.midi[e.track]
          signal = tsm.getTimeSignal(id)
          signal.dom.click()

          if not last_track_val[e.track] then last_track_val[e.track] =  p
          if not orig_signal[e.track] then orig_signal[e.track] =  signal.signal


          # diff = p - last_track_val[e.track]
          # console.log "DIFF", diff, p
          # signal.form = {signal: numeric.add(signal.signal, diff)}
          signal.form = {signal: numeric.mul(orig_signal[e.track], p)}
          last_track_val[e.track] =  p
      when "knob2"
        p = e.value / 128
        if e.track == 7
          signal = tsm.getActiveTimeSignal()
          signal.form = {period: p * 8000}
        if id = theatre.midi[e.track]
          signal = tsm.getTimeSignal(id)
          signal.dom.click()
          signal.form = {period: p * 8000}
      when "pad"      
        # RESET PAD
        if e.track == 7
          if e.value == 127
            resetMIDI()
          return
      
        if id = theatre.midi[e.track]
          signal = tsm.getTimeSignal(id)
          signal.dom.click()
        else if signal = tsm.getActiveTimeSignal()
          ctrl.led(e.track, "light amber")
          theatre.midi[e.track] = signal.id

      when "cursor"
        switch e.direction
          when "down" 
            theatre.state =  PLAY_ONCE
          when "right" 
            theatre.state =  PLAY_REPEAT
          when "left" 
            theatre.state =  HOLD_PLAY
          when "up" 
            theatre.state =  DEMO
        $('.theatre-mode').trigger("theatre_change", theatre.state)
  signalSelectBehavior = (e, step)->
    if tsm.getActiveTimeSignal()
      console.log "Selecting signals"
      signals = $('datasignal')
      i = signals.index($('datasignal.selected'))
      console.log(i)
      i = i + step
      if i < 0 then return
      if i >= signals.length then return
      signals.eq(i).click()
  defaultMIDI = ()->
    ctrl.led 'all', 'off'
    ctrl.led(7, "light amber")
    ctrl.led(1, "light green")
    ctrl.led(0, "light red")
    theatre.midi[0] = tsm.resolve($('#view-4').parent()).id
    theatre.midi[1] = tsm.resolve($('#view-5').parent()).id
  resetMIDI = ()->
    console.log "RESETING MIDI"
    _.each orig_signal, (sig, track)->
      if not sig then return
      if id = theatre.midi[track]
        signal = tsm.getTimeSignal(id)
        signal.form = {signal: sig}
    theatre.midi = {}
    defaultMIDI()
    alertify.notify("<b> RESET THE MIDI CONTROLLER </b> <br>Touch a pad to store a signal.")
    $('#nuke').click()    
