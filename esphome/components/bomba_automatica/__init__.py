# external_components/bomba_automatica/__init__.py

import esphome.config_validation as cv
from esphome.components import Component
from esphome.components import switch

# Definir el esquema de configuración
BOMBA_AUTOMATICA_SCHEMA = cv.Schema({
    cv.Required("id"): cv.string,
    cv.Optional("espera_time", default=300): cv.positive_int,
})

# Definir la función de configuración
def setup_bomba_automatica(config):
    """Configura el componente bomba_automatica"""
    from esphome.components.bomba_automatica import BombaAutomatica
    component = BombaAutomatica()
    component.set_bomba_id(config['id'])
    component.set_espera_time(config.get('espera_time', 300))
    return component

# Registrar el componente en el esquema
esphome.config_validation.register_component('bomba_automatica', BOMBA_AUTOMATICA_SCHEMA, setup_bomba_automatica)
