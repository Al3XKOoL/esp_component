# external_components/bomba_automatica/__init__.py

from esphome import config_validation as cv
from esphome.components import component
from esphome.components.component import Component
import esphome.config_validation as cv

# Definir el esquema de configuración
BOMBA_AUTOMATICA_SCHEMA = cv.Schema({
    cv.Required("id"): cv.string,
    cv.Optional("espera_time", default=300): cv.positive_int,
})

# Definir la función de configuración
def setup_bomba_automatica(config):
    """Configura el componente bomba_automatica"""
    from esphome.components import bomba_automatica as component
    return component.BombaAutomatica(
        bomba_id=config['id'],
        espera_time=config.get('espera_time', 300)
    )

# Registrar el componente en el esquema
esphome.config_validation.register_component('bomba_automatica', BOMBA_AUTOMATICA_SCHEMA, setup_bomba_automatica)

