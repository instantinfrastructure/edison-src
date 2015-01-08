/*
 * Copyright (c) 2014, Intel Corporation.
 *
 * This program is free software; you can redistribute it and/or modify it
 * under the terms and conditions of the GNU Lesser General Public License,
 * version 2.1, as published by the Free Software Foundation.
 *
 * This program is distributed in the hope it will be useful, but WITHOUT ANY
 * WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
 * FOR A PARTICULAR PURPOSE.  See the GNU Lesser General Public License for
 * more details.
 */

/** @module config/pluginManager */

var path = require('path');
var fs = require('fs');

var config = require('./config-manager.js').config;

/**
 * Check if plugin file contains all the member variables defined in the interface. It is the plugin
 * that specifies which interface it wants to adhere to.
 * @param pluginPrototype {object} The '.prototype' member of the plugin object ({@tutorial plugin})
 * @param pluginFile {string} Name of plugin file ({@tutorial plugin})
 * @param interfaceSpec {object} The interface specification object the plugin claims to adhere to
 * ({@tutorial plugin})
 */
function validateProperties(pluginPrototype, pluginFile, interfaceSpec) {
  if(!interfaceSpec.properties) {
    return;
  }

  // check if all required properties exist
  for (var i in interfaceSpec.properties) {
    if (!pluginPrototype[interfaceSpec.properties[i]]) {
      throw new Error("Plugin '" + pluginFile + "' does not define required property '" + interfaceSpec.properties[i] + "'.");
    }
  }
}

/**
 * Check if plugin file contains all the methods defined in the interface. It is the plugin
 * that specifies which interface it wants to adhere to.
 * @param pluginPrototype {object} The '.prototype' member of the plugin object ({@tutorial plugin})
 * @param pluginFile {string} Name of plugin file ({@tutorial plugin})
 * @param interfaceSpec {object} The interface specification object the plugin claims to adhere to
 * ({@tutorial plugin})
 */
function validateFunctions(pluginPrototype, pluginFile, interfaceSpec) {
	if(!interfaceSpec.functions) {
		return;
	}
	
	// check if all required function names exist
	for (var j in interfaceSpec.functions) {
		if (!pluginPrototype[interfaceSpec.functions[j]]) {
			throw new Error("Plugin file '" + pluginFile + "' does not define required function '" + interfaceSpec.functions[j] + "'.");
		}
	}
	
	// check that type is indeed a function
	for (var i in interfaceSpec.functions) {
		if ((typeof pluginPrototype[interfaceSpec.functions[i]]) !== "function") {
			throw new Error("Plugin file '" + pluginFile + "' does not define '" + interfaceSpec.functions[i] + "' as a function.");
		}
	}
}

/**
 * Get the interface file the plugin requests to adhere to.
 * @param pluginPrototype {object} The '.prototype' member of the plugin object ({@tutorial plugin})
 * @param pluginName {string} plugin name ({@tutorial plugin})
 * @returns {string} Absolute path of plugin interface file ({@tutorial plugin})
 */
function getPluginInterfaceFilePath(pluginPrototype, pluginName) {
  var foundPath = "";
  var found = config.pluginInterfaceDirPaths.some(function (pluginInterfaceDirPath) {
    var currPath = path.join(pluginInterfaceDirPath, pluginPrototype.interface + ".json");
    if (fs.existsSync(currPath)) {
      foundPath = currPath;
      return true;
    }
  });

  if (!found) {
    throw new Error("Could not find plugin interface file '" + pluginPrototype.interface + ".json" +
      "' required by plugin '" + pluginName + "' in any of the configured plugin interface directories.");
  }

  return foundPath;
}

/**
 * Find the plugin directory
 * @param pluginName {string} plugin name ({@tutorial plugin})
 * @returns {string} Absolute path of plugin directory ({@tutorial plugin})
 */
function getPluginDirectoryPath(pluginName) {
  "use strict";
  var foundPath = "";
  var found = config.pluginDirPaths.some(function (pluginDirPath) {
    var currPath = path.join(pluginDirPath, pluginName);
    if (fs.existsSync(currPath)) {
      foundPath = currPath;
      return true;
    }
  });

  if (!found) {
    throw new Error("Could not find plugin '" + pluginName + "' in any of the configured plugin directories.");
  }

  return foundPath;
}

/**
 * Loaded plugin objects ({@tutorial plugin}).
 * e.g. loadedPlugins[name of plugin]['client' or 'server'] returns server or client-side plugin object
 * @type {object.<object, object>}
 */
exports.loadedPlugins = {};

/** This library's config file object */
exports.config = require('./config-manager.js').config;

/**
 * Load a plugin. A specific plugin is loaded only when clients are trying to connect to a service that
 * uses the communication plugin, or when a service using the communication plugin starts ({@tutorial plugin}).
 * @param pluginName {string} name of plugin
 */
exports.loadPlugin = function(pluginName)
{
  var emptyPlugin = true;
  var pluginDirectoryPath = getPluginDirectoryPath(pluginName);
  var suffixKeys = Object.keys(config.communicationPlugins.fileSuffixes);

	for (var i = 0; i < suffixKeys.length; i++) {
    var suffix = config.communicationPlugins.fileSuffixes[suffixKeys[i]];
    var pluginFileName = pluginName + '-' + suffix + '.js';
    var pluginFilePath = path.join(pluginDirectoryPath, pluginFileName);

    if (!fs.existsSync(pluginFilePath)) {
      continue;
    }

    emptyPlugin = false;

		var plugin = require(pluginFilePath);
		var pluginPrototype = null;

		if (typeof plugin === "function" && plugin.prototype)
		{ // if pluginPrototype is defined as a class with module.exports = <constructor name>
				pluginPrototype = plugin.prototype;
		} else {
      throw ("Plugin " + pluginName + " must be defined like a class using the prototype object. " +
         "Its functionality should be exported using the module.exports = Constructor() convention.");
    }

		var superInterfaceSpec = JSON.parse(fs.readFileSync(config.superInterfaceFilePath));

		// check if interface contains properties and functions that all interfaces are required to have
		// need to do this here since pluginPrototype.interface is needed below
		validateProperties(pluginPrototype, pluginFileName, superInterfaceSpec);
		validateFunctions(pluginPrototype, pluginFileName, superInterfaceSpec);

		var pluginInterfaceFilePath = getPluginInterfaceFilePath(pluginPrototype, pluginName);
		var pluginInterfaceSpec = JSON.parse(fs.readFileSync(pluginInterfaceFilePath));

    validateProperties(pluginPrototype, pluginFileName, pluginInterfaceSpec);
    validateFunctions(pluginPrototype, pluginFileName, pluginInterfaceSpec);

    if (!exports.loadedPlugins[pluginName]) {
      exports.loadedPlugins[pluginName] = {};
    }

    if (exports.loadedPlugins[pluginName][suffix]) {
      console.log("INFO: Plugin file '" + pluginName + '-' + suffix + ".js' already loaded. Skipping...");
      continue;
    }

    exports.loadedPlugins[pluginName][suffix] = plugin;
	}

  if (emptyPlugin) {
    console.log("WARN: No plugin files were found. Empty plugin directory?");
  }
};

/**
 * Plugins have a client-side component and a server-side component.
 * Load the client-side component. ({@tutorial plugin})
 * @param name {string} plugin name
 * @returns {object} client-side plugin object ({@tutorial plugin})
 */
exports.getClientPlugin = function (name) {
  if (!exports.loadedPlugins[name]) {
    this.loadPlugin(name);
  }

  return exports.loadedPlugins[name][config.communicationPlugins.fileSuffixes.clientFileSuffix];
};

/**
 * Plugins have a client-side component and a server-side component.
 * Load the client-side component. ({@tutorial plugin})
 * @param name {string} plugin name
 * @returns {object} server-side plugin object ({@tutorial plugin})
 */
exports.getServicePlugin = function (name) {
  if (!exports.loadedPlugins[name]) {
    this.loadPlugin(name);
  }

  return exports.loadedPlugins[name][config.communicationPlugins.fileSuffixes.serverFileSuffix];
};

