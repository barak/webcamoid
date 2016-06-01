function Component()
{
}

Component.prototype.createOperations = function()
{
    component.createOperations();

    if (systemInfo.productType === "windows") {
        // Create shortcuts.
        var installDir = ["@TargetDir@", "@StartMenuDir@", "@DesktopDir@"];

        for (var dir in installDir)
            component.addOperation("CreateShortcut",
                                   "@TargetDir@\\bin\\webcamoid.exe",
                                   installDir[dir] + "\\webcamoid.lnk");
    }
}
