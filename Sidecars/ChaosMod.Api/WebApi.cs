using Microsoft.AspNetCore.Builder;
using Microsoft.AspNetCore.Hosting;
using Microsoft.Extensions.DependencyInjection;
using Microsoft.Extensions.Hosting;

namespace ChaosMod.Api;

public class WebApi
{
    private readonly WebApplication _app;
    public WebApi()
    {
        var builder = WebApplication.CreateBuilder(Environment.GetCommandLineArgs());

        builder.Services.AddEndpointsApiExplorer();
        builder.Services.AddControllers();
        builder.Services.AddSwaggerGen();
        builder.WebHost.UseKestrel();
        builder.WebHost.UseUrls("https://localhost:5656");

        _app = builder.Build();
    }

    public async Task RunAsync(CancellationToken ct)
    {
        _app.UseHttpsRedirection();
        _app.UseSwagger();
        _app.UseSwaggerUI(swaggerGenOptions => { swaggerGenOptions.SwaggerEndpoint("/swagger/v1/swagger.json", "FLChaosMod"); });
        _app.MapGet("/", () => "Server online");
        _app.MapControllers();

        await _app.RunAsync(ct);
    }
}