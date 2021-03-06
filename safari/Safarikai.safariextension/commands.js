// Generated by CoffeeScript 1.6.3
(function() {
  window.Commands = {
    toggle: {
      invoke: function(event) {
        Safarikai.toggle();
        return event.target.validate();
      },
      validate: function(event) {
        event.target.toolTip = Safarikai.enabled() ? "Disable Safarikai" : "Enable Safarikai";
        return event.target.image = safari.extension.baseURI + (Safarikai.enabled() ? "IconEnabled.png" : "IconDisabled.png");
      }
    }
  };

}).call(this);
