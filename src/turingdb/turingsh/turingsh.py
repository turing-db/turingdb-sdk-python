"""
TuringDB Shell Client
"""

import click
from prompt_toolkit import prompt
from prompt_toolkit.completion import WordCompleter
from prompt_toolkit.history import InMemoryHistory
from prompt_toolkit.shortcuts import print_formatted_text
from prompt_toolkit.formatted_text import HTML

from . import greeter
from turingdb import TuringDB, TuringDBException


class ShellException(Exception):
    pass


def create_completer():
    """Create command completer"""
    commands = [
        # Shell keywords
        "cd",
        "LIST_AVAIL_GRAPHS",
        "LIST_LOADED_GRAPHS",
        "help",
        "quit",
        "status",
        # Query keywords
        "DESCENDING",
        "CONSTRAINT",
        "MANDATORY",
        "ASCENDING",
        "OPTIONAL",
        "CONTAINS",
        "DISTINCT",
        "EXTRACT",
        "REQUIRE",
        "COLLECT",
        "STARTS",
        "UNIQUE",
        "FILTER",
        "SINGLE",
        "SCALAR",
        "UNWIND",
        "REMOVE",
        "RETURN",
        "CREATE",
        "DELETE",
        "DETACH",
        "EXISTS",
        "IS NOT",
        "LIMIT",
        "YIELD",
        "MATCH",
        "MERGE",
        "ORDER",
        "WHERE",
        "UNION",
        "FALSE",
        "COUNT",
        "DESC",
        "CALL",
        "NULL",
        "TRUE",
        "WHEN",
        "NONE",
        "THEN",
        "ELSE",
        "CASE",
        "ENDS",
        "DROP",
        "SKIP",
        "WITH",
        "ANY",
        "SET",
        "ALL",
        "ASC",
        "NOT",
        "END",
        "XOR",
        "FOR",
        "ADD",
        "AND",
        "OR",
        "IN",
        "IS",
        "BY",
        "DO",
        "OF",
        "ON",
        "IF",
        "AS",
    ]
    return WordCompleter(commands, ignore_case=True)


@click.group(invoke_without_command=True)
@click.option("--host", "-l", default="https://engines.turingdb.ai/sdk")
@click.option("--auth-token", "-p", default="")
@click.option("--instance-id", "-i", default="")
@click.pass_context
def main(ctx, host, auth_token, instance_id):
    """TuringDB Shell - Interactive database client"""
    client = TuringDB(
        host=host,
        auth_token=auth_token,
        instance_id=instance_id,
    )

    start_shell(client)


def start_shell(client: TuringDB):
    """Start the interactive shell"""
    history = InMemoryHistory()
    completer = create_completer()

    # Welcome message
    greeter.greet()

    try:
        client.try_reach()
        client.warmup()
    except Exception as e:
        print_formatted_text(HTML(f"<red>✘ Could not connect to TuringDB.\n{e}</red>"))
        print()
        return

    print_formatted_text(HTML("<green>✓ Connected to TuringDB</green>"))
    print()

    shell_commands = {
        "cd": change_graph,
        "list_avail_graphs": list_available_graphs,
        "list_loaded_graphs": list_loaded_graphs,
    }

    while True:
        graph = client.get_graph()
        try:
            # Get user input with styling
            user_input = prompt(
                HTML(f"<b><cyan>turingdb:{graph}</cyan></b><gray>></gray> "),
                completer=completer,
                history=history,
                multiline=False,
            )

            # Handle commands
            cmd = user_input.strip()

            if not cmd:
                continue

            cmd_lower = cmd.lower()
            cmd_words = cmd_lower.split()

            if len(cmd_words) == 0:
                continue

            if len(cmd_words) == 1:
                if cmd_lower == "quit":
                    print_formatted_text(HTML("<yellow>Goodbye! 👋</yellow>"))
                    break
                elif cmd_lower == "help":
                    show_help()
                    continue

            if cmd_words[0] in shell_commands:
                try:
                    shell_commands[cmd_words[0]](client, cmd_words)
                except ShellException as e:
                    print_formatted_text(HTML(f"<red>✘ {e}</red>"))
                continue

            # Execute query
            try:
                result = client.query(cmd)
                print_formatted_text(HTML(f"<white>{result}</white>"))
            except TuringDBException as e:
                print_formatted_text(HTML(f"<red>✘ {e}</red>"))
            except Exception as e:
                print_formatted_text(HTML(f"<red>✘ Fatal error: {e}</red>"))

            exec_time = client.get_total_exec_time()
            if exec_time is not None:
                print_formatted_text(
                    HTML(
                        f"<white>Total execution time: <yellow>{exec_time:.3f}</yellow> milliseconds</white>"
                    )
                )
            query_time = client.get_query_exec_time()
            if query_time is not None:
                print_formatted_text(
                    HTML(
                        f"<white>Including query execution time: <yellow>{query_time:.3f}</yellow> milliseconds</white>"
                    )
                )

            print()  # Add spacing

        except KeyboardInterrupt:
            print_formatted_text(
                HTML('\n<yellow>Use "quit" or Ctrl+D to quit</yellow>')
            )
            continue
        except EOFError:
            print_formatted_text(HTML("\n<yellow>Goodbye! 👋</yellow>"))
            break


def show_help():
    """Display help information"""
    help_text = """<b>Available Commands:</b>
    <cyan>cd my_graph</cyan>               - Change active graph
    <cyan>LIST_AVAIL_GRAPHS</cyan>         - List available graphs
    <cyan>LIST_LOADED_GRAPHS</cyan>        - List loaded graphs
    <cyan>help</cyan>                      - Show this help
    <cyan>EXIT</cyan> or <cyan>\\q</cyan>  - Quit shell

<b>Example queries:</b>
    <cyan>LOAD GRAPH my_graph</cyan>       - Load graph
    <cyan>MATCH (n) RETURN n.name</cyan>   - Return all node names
    
<b>Tips:</b>
    • Use <i>Tab</i> for command completion
    • Use <i>↑/↓</i> arrows for command history"""

    print_formatted_text(HTML(help_text))


def change_graph(client: TuringDB, args: list[str]):
    """Change the current graph"""

    if len(args) != 2:
        raise ShellException(
            "change_graph() missing 1 required positional argument: 'graph_name'"
        )

    graph_name = args[1]

    try:
        client.set_graph(graph_name)
        if not client.is_graph_loaded():
            print_formatted_text(
                HTML(
                    f"<red>✘ Graph {graph_name} needs to be loaded with 'load graph \"{graph_name}\"'</red>"
                )
            )
            client.set_graph("default")
        else:
            print_formatted_text(
                HTML(f"<green>✓ Changed graph to {graph_name}</green>")
            )
    except TuringDBException as e:
        print_formatted_text(HTML(f"<red>✘ {e}</red>"))
    except Exception as e:
        print_formatted_text(HTML(f"<red>✘ Fatal error: {e}</red>"))


def list_available_graphs(client: TuringDB, args: list[str]):
    """List available graphs"""

    if len(args) != 1:
        raise ShellException(
            "list_available_graphs() missing 0 required positional argument"
        )

    try:
        graphs = client.list_available_graphs()
        print_formatted_text(HTML(f"<white>{graphs}</white>"))
    except TuringDBException as e:
        print_formatted_text(HTML(f"<red>✘ {e}</red>"))
    except Exception as e:
        print_formatted_text(HTML(f"<red>✘ Fatal error: {e}</red>"))


def list_loaded_graphs(client: TuringDB, args: list[str]):
    """List loaded graphs"""

    if len(args) != 1:
        raise ShellException(
            "list_loaded_graphs() missing 0 required positional argument"
        )

    try:
        graphs = client.list_loaded_graphs()
        print_formatted_text(HTML(f"<white>{graphs}</white>"))
    except TuringDBException as e:
        print_formatted_text(HTML(f"<red>✘ {e}</red>"))
    except Exception as e:
        print_formatted_text(HTML(f"<red>✘ Fatal error: {e}</red>"))


if __name__ == "__main__":
    main()
