# external_components/bomba_automatica/__init__.py

from esphome.components import Component
from esphome.config_validation import cv, ConfigType
import esphome.config_validation as cv
from esphome import core

def validate_bomba_automatica_config(config):
    """Validar la configuración del componente bomba_automatica"""
    return config

BOMBA_AUTOMATICA_SCHEMA = cv.Schema({
    cv.Required("id"): cv.string,
    cv.Optional("espera_time", default=300): cv.positive_int,
})

def setup_bomba_automatica(config, component):
    """Configura el componente bomba_automatica"""
    bomba_id = config['id']
    espera_time = config.get('espera_time', 300)
    component.set_bomba_id(bomba_id)
    component.set_espera_time(espera_time)

def setup_component(config):
    """Registra el componente bomba_automatica"""
    from esphome.components import component
    component = component.BombaAutomatica()
    setup_bomba_automatica(config, component)
    return component

# Registrar el componente en el esquema
esphome.config_validation.register_component('bomba_automatica', BOMBA_AUTOMATICA_SCHEMA, setup_component)
