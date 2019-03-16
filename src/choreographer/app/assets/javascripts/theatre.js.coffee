window.DEFAULT_SIGNAL = '#view-9'
window.DEMO = 3
window.PLAY_ONCE = 0
window.PLAY_REPEAT = 1
window.HOLD_PLAY = 2
window.DEFAULT_MODE = HOLD_PLAY

window.setupTheatre = (theatre)->
  theatre.state = DEFAULT_MODE;
  $('.theatre-mode').trigger('theatre_change', theatre.state)
  # DEFAULT SIGNAL
  $(DEFAULT_SIGNAL).click()

  theatre.processTheatre = ()->
    theatre.midi = {}
    theatre.docks = []
    theatre.stages = []
    scope = this
    _.each this.getItems({selectable: true}), (item)->
      if item.name
        flag = item.name.split(":")[0]
        switch flag
          when "RX"
            item.data.dock_id = theatre.docks.length
            theatre.docks.push(item)
          when "TX"
            item.data.stage_id = theatre.stages.length
            item.data.name = CanvasUtil.getName(item)
            theatre.stages.push(item)
    return {
      stages: theatre.stages
      docks: theatre.docks
    }
  theatre.clearDocks = ()->
    # SEND ALL DOCKS OFF SIGNAL
    docks = [0, 1, 2]
    _.each docks, (d_id)->
      id = sc.sendMessage {flag: 'v', args: [d_id, 0]},
        delay: 0
        live: true
        update: false
    # REVERT STAGES TO STAGE COLOR
    _.each theatre.stages, (stage)->
      if stage.data and stage.data.color
        c = new paper.Color(stage.data.color)
        id = sc.sendMessage {flag: 'l', args: [stage.data.dock, parseInt(c.red * 255),  parseInt(c.green * 255),  parseInt(c.blue * 255)]},
          delay: 100
          live: true
          update: false

  theatre.stopDocks = ()->  
    # FLAG OFF STAGE REPETITION      
    # CLEAR ALL PLAY IDS AND TIMEOUT MANAGER IDS
    _.each theatre.stages, (stage)->
      if stage.repeat
          stage.repeat = false
      if stage.play_ids
        _.each stage.play_ids, (id)->
          clearTimeout(id)

    _.each window.timeoutManager, (id)->
      clearTimeout(id)

    window.timeoutManager = []

  theatre.sendPWM = (dock_id, stage, voltage)->
    if _.isUndefined dock_id
      alertify.error("Redock needed")
      return
    revert = (t)->
      c = new paper.Color(stage.data.color)
      id = sc.sendMessage {flag: 'l', args: [dock_id, parseInt(c.red * 255),  parseInt(c.green * 255),  parseInt(c.blue * 255)]},
          delay: t
          live: true
          update: false

    # INTERACTION LOGIC
    switch theatre.state
      when DEMO
        voltage = parseInt(voltage * 4095)
        sc.sendMessage {flag: 'v', args: [dock_id, voltage]},
          delay: 0
          live: true
          update: false
        if voltage == 0 then revert(0)
      when HOLD_PLAY
        if signal = tsm.getActiveTimeSignal()
          if voltage == 1
            commands = signal.command_list()

            stage.play_ids = _.map commands, (command)->
              voltage = parseInt(command.param * 4095)
              id = sc.sendMessage {flag: 'v', args: [dock_id, voltage]},
                delay: command.t
                live: true
                update: false
              return id
          else
            _.each stage.play_ids, (id)->
              # console.log "clearing", id
              clearTimeout(id)
            stage.play_ids = []
            sc.sendMessage {flag: 'v', args: [dock_id, 0]},
              delay: 0
              live: true
              update: false
            revert(0)
        else
          alertify.error('Select a signal to send.')
      when PLAY_ONCE
        if signal = tsm.getActiveTimeSignal()
          commands = signal.command_list()

          play_ids = _.map commands, (command)->
            voltage = parseInt(command.param * 4095)
            id = sc.sendMessage {flag: 'v', args: [dock_id, voltage]},
              delay: command.t
              live: true
              update: false
            window.timeoutManager.push id
          revert(signal.period);
        else
          alertify.error('Select a signal to send.')
      when PLAY_REPEAT
        if voltage != 1 then return

        if signal = tsm.getActiveTimeSignal()
          stage.repeat = !stage.repeat
        else
          alertify.error('Select a signal to send.')  


        if stage.repeat
          stage.play_ids = []

          behavior = ()->
            commands = signal.command_list()
            _.each commands, (command)->
              voltage = parseInt(command.param * 4095)
              id = sc.sendMessage {flag: 'v', args: [dock_id, voltage]},
                delay: command.t
                live: true
                update: false
              window.timeoutManager.push id
              stage.play_ids.push id
            
            # LINK TO THE NEXT ITERATION OF THE BEHAVIOR
            if stage.repeat
              b_id = _.delay(behavior, signal.period)
              stage.play_ids.push(b_id)
              window.timeoutManager.push(b_id)

          # INITIATE BEHAVIOR
          if stage.repeat
            behavior()
        else
          _.each stage.play_ids, (id)->
            clearTimeout(id)
          stage.play_ids = []

          # CLEAR AND RESET STAGE
          sc.sendMessage {flag: 'v', args: [dock_id, 0]},
            delay: 0
            live: true
            update: false
          revert(0)
      


  theatre.updateStage = (stage_id, up)->
    stage = theatre.getItem
      data: 
        stage_id: stage_id
    pad = stage.getItem
      name: (x)->
        x.startsWith("stage")
    
    # RELEASED
    if pad and up
      pad.set
        strokeColor: stage.data.color
        strokeWidth: 10 
      this.sendPWM(stage.data.dock, stage, 0)

    # CONTACT
    if pad and !up
      pad.set
        strokeColor: "white"
        strokeWidth: 10 

      this.sendPWM(stage.data.dock, stage, 1.0)


  theatre.updateDock = (dock_id, stage_id, color)->
    console.log "UD", dock_id, stage_id, color
    dock = theatre.getItem {data: {dock_id: dock_id}}
    stage = theatre.getItem {data: {stage_id: stage_id}}

    if dock.data.stage then dock.data.stage.visible = false

    stage.visible = true
    dock.data.stage = stage
    stage.data.dock = dock_id
    stage.data.color = color

    stage.position = dock.position.clone()
    pad = stage.getItem
      name: (x)->
        x.startsWith("stage")
    if pad
      pad.set
        strokeColor: stage.data.color
        strokeWidth: 10 
  t = theatre.processTheatre()
  console.log "✓", "STAGES:", t.stages.length
  console.log "✓", "DOCKS:", t.docks.length