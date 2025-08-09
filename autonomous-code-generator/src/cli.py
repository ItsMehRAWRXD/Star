"""Command-line interface for the autonomous code generator"""

import click
import sys
from pathlib import Path
from rich.console import Console
from rich.table import Table
from rich.syntax import Syntax

from .core.generator_manager import GeneratorManager
from .core.generator_registry import registry

console = Console()


@click.group()
@click.version_option(version='1.0.0')
def cli():
    """Autonomous Code Generator - Generate unlimited code in any language"""
    pass


@cli.command()
@click.option('--generator', '-g', required=True, help='Generator to use (e.g., simple, pattern, function)')
@click.option('--language', '-l', default='python', help='Target language')
@click.option('--output', '-o', help='Output file path')
@click.option('--count', '-c', default=1, type=int, help='Number of pieces to generate (-1 for unlimited)')
@click.option('--pattern-type', '-p', help='Pattern type for pattern generators')
@click.option('--config', '-cfg', help='Configuration file path')
@click.pass_context
def generate(ctx, generator, language, output, count, pattern_type, config):
    """Generate code using specified generator"""
    
    manager = GeneratorManager(config)
    
    try:
        # Build kwargs
        kwargs = {
            'language': language,
        }
        
        if pattern_type:
            kwargs['pattern_type'] = pattern_type
            
        # Generate code
        if count == -1:
            console.print(f"[yellow]Starting unlimited generation with {generator}...[/yellow]")
            console.print("[red]Press Ctrl+C to stop[/red]")
            
            try:
                gen = manager.generate(generator, count=count, **kwargs)
                i = 0
                for code in gen:
                    i += 1
                    if output:
                        file_path = Path(output).parent / f"{Path(output).stem}_{i:06d}{Path(output).suffix}"
                        with open(file_path, 'w') as f:
                            f.write(code)
                        console.print(f"[green]Generated {file_path}[/green]")
                    else:
                        console.print(f"\n[bold cyan]--- Generated Code #{i} ---[/bold cyan]")
                        syntax = Syntax(code, language, theme="monokai", line_numbers=True)
                        console.print(syntax)
                        
            except KeyboardInterrupt:
                console.print(f"\n[yellow]Stopped after generating {i} pieces[/yellow]")
                
        else:
            result = manager.generate(generator, output_path=output, count=count, **kwargs)
            
            if output:
                console.print(f"[green]✓ Generated code saved to {output}[/green]")
            else:
                if isinstance(result, list):
                    for i, code in enumerate(result):
                        console.print(f"\n[bold cyan]--- Generated Code #{i+1} ---[/bold cyan]")
                        syntax = Syntax(code, language, theme="monokai", line_numbers=True)
                        console.print(syntax)
                else:
                    syntax = Syntax(result, language, theme="monokai", line_numbers=True)
                    console.print(syntax)
                    
    except Exception as e:
        console.print(f"[red]Error: {e}[/red]")
        ctx.exit(1)


@cli.command()
def list():
    """List available generators"""
    
    manager = GeneratorManager()
    generators = manager.list_generators()
    
    if not generators:
        console.print("[yellow]No generators available[/yellow]")
        return
        
    table = Table(title="Available Generators")
    table.add_column("Name", style="cyan")
    table.add_column("Type", style="magenta")
    table.add_column("Languages", style="green")
    table.add_column("Description", style="white")
    
    for gen in generators:
        langs = ", ".join(gen['languages'][:3])
        if len(gen['languages']) > 3:
            langs += "..."
        table.add_row(
            gen['name'],
            gen['type'],
            langs,
            gen['description']
        )
        
    console.print(table)


@cli.command()
@click.argument('generator')
def info(generator):
    """Show detailed information about a generator"""
    
    manager = GeneratorManager()
    
    try:
        info = manager.get_generator_info(generator)
        
        console.print(f"\n[bold cyan]Generator: {generator}[/bold cyan]")
        console.print(f"Type: {info['type']}")
        console.print(f"Description: {info['description']}")
        console.print(f"Supported Languages: {', '.join(info['languages'])}")
        
        if info.get('config'):
            console.print("\n[bold]Configuration:[/bold]")
            for key, value in info['config'].items():
                console.print(f"  {key}: {value}")
                
    except Exception as e:
        console.print(f"[red]Error: {e}[/red]")


@cli.command()
def examples():
    """Show example usage"""
    
    examples_text = """
[bold cyan]Example Usage:[/bold cyan]

# Generate simple Assembly code
codegen generate -g simple -l asm

# Generate Python design patterns
codegen generate -g pattern -p singleton

# Generate unlimited functions
codegen generate -g function -c -1 -o output/func.py

# Generate fractal code structures
codegen generate -g fractal -p tree

# Generate alphanumeric patterns
codegen generate -g simple -p pattern -l c

# List all generators
codegen list

# Get info about a generator
codegen info pattern
"""
    console.print(examples_text)


def main():
    """Main entry point"""
    # Auto-discover generators
    try:
        registry.auto_discover("src.generators")
    except Exception as e:
        console.print(f"[yellow]Warning: Failed to auto-discover generators: {e}[/yellow]")
    
    cli()


if __name__ == "__main__":
    main()