"use strict";

function initFlameGraphs() {
    var flameGraphs = document.getElementsByClassName("flamegraph");
    Array.prototype.forEach.call(flameGraphs, function(flameGraph) {
        initFlameGraph(flameGraph);
    });
}

function initFlameGraph(flameGraph) {
    flameGraph.addEventListener("click", function(e) {
        var target = findGroup(e.target, flameGraph);
        if (target) {
            // Logik für Klick-Events (Zoom etc.)
        }
    });

    // Event-Listener für Mouseover und Mouseout hinzufügen
    // Andere Event-Listener und Logik hinzufügen
}

function findGroup(node, flameGraph) {
    var parent = node.parentElement;
    if (!parent) return null;
    if (parent === flameGraph) return node;
    return findGroup(parent, flameGraph);
}

// Funktionen für Suchen, Zoomen und weitere Interaktivität

window.onload = initFlameGraphs;
