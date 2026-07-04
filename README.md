# ESPHome Xantrex Custom Component (Modernised)

(Vibecode warning, use with caution)

This repository is a fork of the original Xantrex custom component, updated to ensure compatibility with modern ESPHome releases (tested and validated on **ESPHome 2026.6+** running **Python 3.13**).

## Why this fork exists

Recent core updates to ESPHome and Python introduced breaking changes to how custom components are initialized and structured. The original implementation caused runtime namespace crashes and compilation failures on modern stacks.

### Key updates in this fork:

- **Directory Structure Reorganisation:** Migrated layout to the standard `custom_components/xantrex/` format to prevent Python module resolution errors.
- **Component Loader Compatibility:** Added a formal `__init__.py` file with proper schema validation to satisfy the current ESPHome core loader requirements.
- **C++ API Modernisation:** Refactored `xantrex.h` to align with modern object-oriented instantiation, moving hardware definitions out of legacy inline C++ logic.
- **Python Configuration Schema:** Updated `sensor.py` to handle all metadata tracking, device classes, state classes, and decimal precision layers directly within the Python build schema.

## Installation & Configuration

To use this modernised component, reference it as an external component in your ESPHome YAML configuration file:

```yaml
external_components:
  - source:
      type: git
      url: [https://github.com/thomas-tank/xantrex](https://github.com/thomas-tank/xantrex)
    components: [ xantrex ]
```
