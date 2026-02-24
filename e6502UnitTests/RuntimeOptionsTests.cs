using System;
using KDS.e6502;
using Microsoft.VisualStudio.TestTools.UnitTesting;

namespace e6502UnitTests;

[TestClass]
public class RuntimeOptionsTests
{
    [TestMethod]
    public void GetIntFromEnvironment_UsesFallbackWhenMissing()
    {
        const string key = "NOVA_TEST_INT_OPTION";
        string? previous = Environment.GetEnvironmentVariable(key);
        Environment.SetEnvironmentVariable(key, null);
        try
        {
            Assert.AreEqual(123, RuntimeOptions.GetIntFromEnvironment(key, 123));
        }
        finally
        {
            Environment.SetEnvironmentVariable(key, previous);
        }
    }

    [TestMethod]
    public void GetIntFromEnvironment_ParsesValidValue()
    {
        const string key = "NOVA_TEST_INT_OPTION";
        string? previous = Environment.GetEnvironmentVariable(key);
        Environment.SetEnvironmentVariable(key, "777");
        try
        {
            Assert.AreEqual(777, RuntimeOptions.GetIntFromEnvironment(key, 123));
        }
        finally
        {
            Environment.SetEnvironmentVariable(key, previous);
        }
    }

    [TestMethod]
    public void GetFlagFromEnvironment_ParsesTruthyValues()
    {
        const string key = "NOVA_TEST_BOOL_OPTION";
        string? previous = Environment.GetEnvironmentVariable(key);
        Environment.SetEnvironmentVariable(key, "yes");
        try
        {
            Assert.IsTrue(RuntimeOptions.GetFlagFromEnvironment(key, fallback: false));
        }
        finally
        {
            Environment.SetEnvironmentVariable(key, previous);
        }
    }

    [TestMethod]
    public void GetFlagFromEnvironment_ParsesFalsyValues()
    {
        const string key = "NOVA_TEST_BOOL_OPTION";
        string? previous = Environment.GetEnvironmentVariable(key);
        Environment.SetEnvironmentVariable(key, "off");
        try
        {
            Assert.IsFalse(RuntimeOptions.GetFlagFromEnvironment(key, fallback: true));
        }
        finally
        {
            Environment.SetEnvironmentVariable(key, previous);
        }
    }
}
